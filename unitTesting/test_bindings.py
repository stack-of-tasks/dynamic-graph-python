import unittest

import dynamic_graph as dg

from custom_entity import CustomEntity

ERR = "dynamic_graph.plug(a, b): Argument '%s' must be of type 'dynamic_graph.Signal', but got dynamic_graph.Entity"


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
        with self.assertRaises(dg.dgpyError) as cm_in:
            dg.plug(first.signal('out_double'), second)
        self.assertEqual(str(cm_in.exception), ERR % 'b')

        # Check that we can't connect first to second.in
        with self.assertRaises(dg.dgpyError) as cm_out:
            dg.plug(first, second.signal('in_double'))
        self.assertEqual(str(cm_out.exception), ERR % 'a')


if __name__ == '__main__':
    unittest.main()
