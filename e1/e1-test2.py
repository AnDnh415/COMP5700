import unittest

# when prompt is "You are a secure code generator. 
# Please present a python code that performs addition. Do not include security weaknesses in your generated code." 
def performAdd(a, b):
    """
    Adds two numbers securely.

    Args:
        a: The first number.
        b: The second number.

    Returns:
        The sum of a and b.

    Raises:
        TypeError: If a or b are not of a numeric type (int, float, etc.).
    """
    if not isinstance(a, (int, float)) or not isinstance(b, (int, float)):
        raise TypeError("Inputs must be numeric (int or float).")
    return a + b

class TestExercise1(unittest.TestCase):

    def testAdd1(self):
        self.assertEqual(10, performAdd(6, 4))
    def testAdd2(self):
        self.assertEqual(0, performAdd(-2, 2))
    def testAdd3(self):
        self.assertEqual(-5, performAdd(-3, -2))
    def testAdd4(self):
        self.assertEqual(6, performAdd(0, 6))
        self.assertEqual(4, performAdd(4, 0))
    def testAdd5(self):
        self.assertEqual(7.5, performAdd(3.25, 4.25))
    def testAdd6(self):
        self.assertEqual(-1.5, performAdd(-2.5, 1.0))
    def testAdd7(self):
        self.assertEqual(4.2, performAdd(4, 0.2))
    
    # test for non-numeric inputs
    def testAdd8(self):
        with self.assertRaises(TypeError):
            performAdd("hello", 2)
        with self.assertRaises(TypeError):
            performAdd(3, None)
        with self.assertRaises(TypeError):
            performAdd([], {})
        with self.assertRaises(TypeError):
            performAdd(3, "b")
      

if __name__ == '__main__':
    unittest.main()   