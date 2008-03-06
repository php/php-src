--TEST--
Test is_a() function : usage variations  - wrong type for arg 2 
--FILE--
<?php
/* Prototype  : proto bool is_a(object object, string class_name)
 * Description: Returns true if the object is of this class or has this class as one of its parents 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

class C {
	function __toString() {
		return "C Instance";
	}
}

echo "*** Testing is_a() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$object = new stdclass();

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

      // int data
      0,
      1,
      12345,
      -2345,

      // float data
      10.5,
      -10.5,
      10.1234567e10,
      10.7654321E-10,
      .5,

      // array data
      array(),
      array(0),
      array(1),
      array(1, 2),
      array('color' => 'red', 'item' => 'pen'),

      // null data
      NULL,
      null,

      // boolean data
      true,
      false,
      TRUE,
      FALSE,

      // empty data
      "",
      '',

      // object data
      new C,

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for class_name

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( is_a($object, $value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing is_a() : usage variations ***

Notice: Undefined variable: undefined_var in %s on line 64

Notice: Undefined variable: unset_var in %s on line 67

Arg value 0 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value 1 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value 12345 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value -2345 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value 10.5 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value -10.5 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value 101234567000 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value 1.07654321E-9 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value 0.5 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value Array 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74

Notice: Array to string conversion in %s on line 74
bool(false)

Arg value Array 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74

Notice: Array to string conversion in %s on line 74
bool(false)

Arg value Array 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74

Notice: Array to string conversion in %s on line 74
bool(false)

Arg value Array 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74

Notice: Array to string conversion in %s on line 74
bool(false)

Arg value Array 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74

Notice: Array to string conversion in %s on line 74
bool(false)

Arg value  

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value  

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value 1 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value  

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value 1 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value  

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value  

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value  

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value C Instance 

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value  

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)

Arg value  

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 74
bool(false)
Done