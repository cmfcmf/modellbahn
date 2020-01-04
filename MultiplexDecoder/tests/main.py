from sys import argv, stdout, exit
import functools
import os
from ex_utils import SimulavrAdapter
import pysimulavr
from testcase import Testcase


class MyTestCase(Testcase):
    def test(self):
        self.run_arduino_setup()

        for pin in self.signal_1_pins:
            pin.SetPin("t")

        # Press programming button for 300ms
        # Does not work, because ADC free running mode with Timer0 trigger is not implemented.
        # self.press_programming_button(0.3 * 10 ** 9)

        self.sim.doRun(self.sim.getCurrentTime() + 3 * 10 ** 9)

        # Does not work, because WDT interrupts are not implemented.
        # for i in range(1, 10):
        #     self.sim.doRun(self.sim.getCurrentTime() + 0.1 * 10 ** 9)
        #     print "value 'quarterSeconds'=%d" % self.sim.getWordByName(self.dev, "quarterSeconds")


if __name__ == "__main__":
    tc = MyTestCase()
    tc.setup()
    tc.test()
    tc.teardown()