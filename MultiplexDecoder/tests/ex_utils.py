import pysimulavr


# Based on a file found in the simulavr examples
# simulavr - A simulator for the Atmel AVR family of microcontrollers.
#  * Copyright (C) 2001, 2002, 2003   Klaus Rudolph
# Licensed under the GPLv2
class SimulavrAdapter(object):
  def loadDevice(self, t, e, f_cpu):
    self.__sc = pysimulavr.SystemClock.Instance()
    self.__sc.ResetClock()
    dev = pysimulavr.AvrFactory.instance().makeDevice(t)
    dev.Load(e)
    dev.SetClockFreq(10 ** 9 / f_cpu)
    self.__sc.Add(dev)
    return dev

  def doRun(self, n):
    ct = self.__sc.GetCurrentTime
    while ct() < n:
      res = self.__sc.Step()
      if res is not 0: return res
    return 0

  def doStep(self, stepcount = 1):
    while stepcount > 0:
      res = self.__sc.Step()
      if res is not 0: return res
      stepcount -= 1
    return 0

  def runUntilBreakpoint(self):
    while True:
      res = self.__sc.Step()
      if res is not 0: return res

  def getCurrentTime(self):
    return self.__sc.GetCurrentTime()

  def getAllRegisteredTraceValues(self):
    os = pysimulavr.ostringstream()
    pysimulavr.DumpManager.Instance().save(os)
    return filter(None, [i.strip() for i in os.str().split("\n")])

  def dmanSingleDeviceApplication(self):
    pysimulavr.DumpManager.Instance().SetSingleDeviceApp()

  def dmanStart(self):
    pysimulavr.DumpManager.Instance().start()

  def dmanStop(self):
    pysimulavr.DumpManager.Instance().stopApplication()

  def setVCDDump(self, vcdname, signals, rstrobe = False, wstrobe = False):
    dman = pysimulavr.DumpManager.Instance()
    sigs = ["+ " + i for i in signals]
    dman.addDumpVCD(vcdname, "\n".join(sigs), "ns", rstrobe, wstrobe)

  def startVCDDumps(self):
    dman = pysimulavr.DumpManager.Instance()
    dman.start()

  def getWordByName(self, dev, label):
    addr = dev.data.GetAddressAtSymbol(label)
    v = dev.getRWMem(addr)
    addr += 1
    v = (dev.getRWMem(addr) << 8) + v
    return v
