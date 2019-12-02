import unittest

import dynamic_graph as dg
from custom_entity import CustomEntity


class BindingsTests(unittest.TestCase):
    def test_bindings(self):
        with self.assertRaises(Exception) as cm:
            dg.error_out()
        self.assertEqual(str(cm.exception), "something bad happened")

    def test_type_check(self):
        first = CustomEntity('first_entity')
        second = CustomEntity('second_entity')
        # Check that we can connect first.out to second.in
        dg.plug(first.signal('out_double'), second.signal('in_double'))
        # Check that we can't connect first.out to second
        with self.assertRaises(ValueError) as cm:
            dg.plug(first.signal('out_double'), second)
        self.assertEqual(str(cm.exception), "PyCapsule_GetPointer called with incorrect name")


if __name__ == '__main__':
    unittest.main()
