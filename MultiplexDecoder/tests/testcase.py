from sys import argv, stdout, exit
import functools
import os
from ex_utils import SimulavrAdapter
import pysimulavr


class XPin(pysimulavr.Pin):
    __on_state_changed = None
    __last_state = None

    def __init__(self, dev, name, state=None):
        pysimulavr.Pin.__init__(self)
        self.name = name
        if state is not None: self.SetPin(state)
        # hold the connecting net here, it have not be destroyed, if we leave this method
        self.__net = pysimulavr.Net()
        self.__net.Add(self)
        self.__net.Add(dev.GetPin(name))

    def set_on_state_changed(self, on_state_changed):
        self.__on_state_changed = on_state_changed

    def SetInState(self, pin):
        """
        :type pin: pysimulavr.Pin
        """
        pysimulavr.Pin.SetInState(self, pin)

        new_state = pin.toChar()
        if new_state != self.__last_state and self.__on_state_changed is not None:
            self.__on_state_changed(new_state=new_state, old_state=self.__last_state)
            self.__last_state = new_state


# Based on the script by Kevin O'Connor from
# https://lists.gnu.org/archive/html/simulavr-devel/2015-05/msg00008.html
#
# Copyright (C) 2015  Kevin O'Connor <address@hidden>
#
# Licensed under the GPLv3.
class SerialRxPin(pysimulavr.PySimulationMember, pysimulavr.Pin):
    def __init__(self, baud):
        pysimulavr.Pin.__init__(self)
        pysimulavr.PySimulationMember.__init__(self)
        self.sc = pysimulavr.SystemClock.Instance()
        self.delay = 2 * 10 ** 9 / baud # TODO: Why *2 ???
        self.current = 0
        self.pos = -1

    def SetInState(self, pin):
        pysimulavr.Pin.SetInState(self, pin)
        self.state = pin.outState
        if self.pos < 0 and pin.outState == pin.LOW:
            self.pos = 0
            self.sc.Add(self)

    def DoStep(self, trueHwStep):
        ishigh = self.state == self.HIGH

        self.current |= ishigh << self.pos
        self.pos += 1

        if self.pos == 1:
            return int(self.delay * 1.5)
        if self.pos >= 10:  # 8N1 = 1 start, 8 data, 1 stop:
            self.handleChar(chr((self.current >> 1) & 0xff))
            self.pos = -1
            self.current = 0
            return -1
        return self.delay

    def handleChar(self, c):
        stdout.write(c)
        stdout.flush()


class DCCPin(pysimulavr.PySimulationMember, pysimulavr.Pin):
    zero_len_half = 116
    one_len_half = 58

    packet_idx = 0
    idle_packet = "1111111111111 0 11111111 0 00000000 0 11111111 1"

    current_bit = int(idle_packet[0])
    bit_state = "first_half"

    def __init__(self, dev):
        pysimulavr.Pin.__init__(self)
        pysimulavr.PySimulationMember.__init__(self)

        # self.dcc_net = pysimulavr.Net()
        # self.dcc_net.Add(self.dccpin)
        # self.dcc_net.Add(dev.GetPin("D2"))
        self.pin = dev.GetPin("D2")


        self.set_pin('L')
        self.sc = pysimulavr.SystemClock.Instance()

        self.sc.Add(self)

    def DoStep(self, trueHwStep):
        if self.current_bit == 0:
          delay = self.zero_len_half * 1000
        elif self.current_bit == 1:
          delay = self.one_len_half * 1000
        else:
          assert False

        if self.bit_state == "first_half":
            self.set_pin("h")
            self.bit_state = "last_half"
        elif self.bit_state == "last_half":
            self.set_pin("l")
            self.bit_state = "first_half"

            while True:
              self.packet_idx += 1
              if self.packet_idx >= len(self.idle_packet):
                self.packet_idx = 0
              next_bit = self.idle_packet[self.packet_idx]
              if next_bit != " ":
                self.current_bit = int(next_bit)
                break
        else:
          assert False

        return delay

    def set_pin(self, state):
        self.pin.SetPin(state)


class MyPinMonitor:
    def __init__(self, sim, dev, idx):
        self.sim = sim
        self.dev = dev
        self.idx = idx
        self.last_change = sim.getCurrentTime()

    def __call__(self, *args, **kwargs):
        new_state = kwargs["new_state"]
        old_state = kwargs["old_state"]
        now = self.sim.getCurrentTime()
        # print "%s %s -> %s %s" % (self.idx, old_state, new_state, (now - self.last_change) / self.dev.GetClockFreq())
        self.last_change = now


class Testcase:
  f_cpu = 16000000

  def setup(self):
    proc = "atmega328"
    elffile = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", "build-nano-atmega328old", "MultiplexDecoder.elf")

    sim = SimulavrAdapter()
    sim.dmanSingleDeviceApplication()
    dev = sim.loadDevice(proc, elffile, self.f_cpu)  # type: pysimulavr.AvrDevice

    self.sim = sim
    self.dev = dev
    self.bp = dev.BP  # type: pysimulavr.Breakpoints
    self.flash = dev.Flash  # type: pysimulavr.AvrFlash

    self.programming_button = pysimulavr.Pin()
    self.programming_button.SetAnalogValue(5 * 1000000)
    self.pb_net = pysimulavr.Net()
    self.pb_net.Add(self.programming_button)
    self.pb_net.Add(dev.GetPin("ADC6"))

    self.aref_pin = pysimulavr.Pin()
    self.aref_pin.SetAnalogValue(5 * 1000000)
    self.aref_net = pysimulavr.Net()
    self.aref_net.Add(self.aref_pin)
    self.aref_net.Add(dev.GetPin("AREF"))

    self.led_pin = XPin(dev, "B5")
    self.led_pin.set_on_state_changed(MyPinMonitor(sim, dev, "led"))


    self.signal_1_pins = []
    for idx, name in enumerate(["D3", "D4", "D5", "D6"]):
        pin = XPin(dev, name)
        pin.set_on_state_changed(MyPinMonitor(sim, dev, idx))
        pin.SetPin("L")
        self.signal_1_pins.append(pin)

    self.rxpin = SerialRxPin(115200)
    self.rx_net = pysimulavr.Net()
    self.rx_net.Add(self.rxpin)
    self.rx_net.Add(dev.GetPin("D1"))

    self.dccpin = DCCPin(dev)

    sim.dmanStart()
    print "simulation start: (t=%dns)" % sim.getCurrentTime()

    self.setup_tracing()

  def setup_tracing(self):
    irqs_to_monitor = [
        1,  # INT0
        6,  # WDT
        21, # ADC
        16, # Timer0 OVF
        11, # Timer1 COMPA
        7   # Timer2 COMPA
    ]
    # You cannot monitor input pints!
    out_pins_to_monitor = [
      "D3", "D4", "D5", "D6", # Signal 1
      "D7", "B0", "B1", "B2", # Signal 2
      "C2", "C3", "C4", "C5", # Signal 3
      "B4", "B3", "C0", "C1", # Signal 4
    ]
    monitored_signals = []
    for irq_idx in irqs_to_monitor:
        monitored_signals.append("IRQ.VECTOR" + str(irq_idx))
    for pin in out_pins_to_monitor:
        monitored_signals.append("PORT" + pin[0] + "." + pin + "-Out")
    self.sim.setVCDDump("/tmp/simulavr_trace.vcd", monitored_signals)
    self.sim.startVCDDumps()

  def press_programming_button(self, duration):
    self.programming_button.SetAnalogValue(0 * 1000000)
    self.sim.doRun(self.sim.getCurrentTime() + duration)
    self.programming_button.SetAnalogValue(5 * 1000000)

  def run_arduino_setup(self):
    # Run until loop() is about to be executed for the first time
    loop_address = self.flash.GetAddressAtSymbol("loop")
    self.bp.AddBreakpoint(loop_address)
    self.sim.runUntilBreakpoint()
    self.bp.RemoveBreakpoint(loop_address)

  def teardown(self):
    print "simulation end: (t=%dns)" % self.sim.getCurrentTime()

    self.sim.dmanStop()
    del self.dev