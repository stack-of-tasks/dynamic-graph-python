import unittest

import dynamic_graph


class BindingsTests(unittest.TestCase):
    def test_bindings(self):
        with self.assertRaises(Exception) as error:
            dynamic_graph.error_out()
            self.assertEqual(str(error), "something bad happend")


if __name__ == '__main__':
    unittest.main()
