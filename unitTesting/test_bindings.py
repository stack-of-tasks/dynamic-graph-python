import unittest

import dynamic_graph as dg

from custom_entity import CustomEntity

ERR = "dynamic_graph.plug(a, b): Argument '%s' must be of type 'dynamic_graph.Signal', but got dynamic_graph.Entity"


class BindingsTests(unittest.TestCase):
    def test_bindings(self):
        with self.assertRaises(dg.dgpyError) as cm:
            dg.error_out()
        self.assertEqual(str(cm.exception), "something bad happened")

    def test_type_check(self):
        """
        test the type checking in signal plugs
        """
        first = CustomEntity('first_entity')
        second = CustomEntity('second_entity')
        # Check that we can connect first.out to second.in
        dg.plug(first.signal('out_double'), second.signal('in_double'))

        # Check that we can't connect first.out to second
        with self.assertRaises(TypeError) as cm_in:
            dg.plug(first.signal('out_double'), second)
        self.assertEqual(str(cm_in.exception), ERR % 'b')

        # Check that we can't connect first to second.in
        with self.assertRaises(TypeError) as cm_out:
            dg.plug(first, second.signal('in_double'))
        self.assertEqual(str(cm_out.exception), ERR % 'a')

    def test_dg_exc(self):
        """
        test that exceptions from dynamic graph are correctly raised
        """
        ent = CustomEntity('test_dg_exc')
        # check that accessing a non initialized signal raises
        with self.assertRaises(dg.dgpyError) as cm:
            ent.act()
        self.assertEqual(
            str(cm.exception),
            'In SignalPtr: SIN ptr not set. (in signal <CustomEntity(test_dg_exc)::input(double)::in_double>)')

        # check that accessing an initialized signal doesn't raise
        ent_2 = CustomEntity('another_entity')
        dg.plug(ent_2.signal('out_double'), ent.signal('in_double'))
        ent.act()


if __name__ == '__main__':
    unittest.main()
