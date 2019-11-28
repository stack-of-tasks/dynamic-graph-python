import unittest

import dynamic_graph as dg
from custom_entity import CustomEntity


class BindingsTests(unittest.TestCase):
    def test_bindings(self):
        with self.assertRaises(Exception) as error:
            dg.error_out()
            self.assertEqual(str(error), "something bad happend")

    def test_type_check(self):
        first = CustomEntity('first_entity')
        second = CustomEntity('second_entity')
        # Check that we can connect first.out to second.in
        dg.plug(first.signal('out_double'), second.signal('in_double'))
        # Check that we can't connect first.out to second
        with self.assertRaises(Exception):
            dg.plug(first.signal('out_double'), second)


if __name__ == '__main__':
    unittest.main()
