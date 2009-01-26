--TEST--
Test var_export() function with valid arrays
--FILE--
<?php
/* Prototype  : mixed var_export(mixed var [, bool return])
 * Description: Outputs or returns a string representation of a variable 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */


echo "*** Testing var_export() with valid arrays ***\n";
// different valid  arrays 
$valid_arrays = array(
           "array()" => array(),
           "array(NULL)" => array(NULL),
           "array(null)" => array(null),
           "array(true)" => array(true),
           "array(\"\")" => array(""),
           "array('')" => array(''),
           "array(array(), array())" => array(array(), array()),
           "array(array(1, 2), array('a', 'b'))" => array(array(1, 2), array('a', 'b')),
           "array(1 => 'One')" => array(1 => 'One'),
           "array(\"test\" => \"is_array\")" => array("test" => "is_array"),
           "array(0)" => array(0),
           "array(-1)" => array(-1),
           "array(10.5, 5.6)" => array(10.5, 5.6),
           "array(\"string\", \"test\")" => array("string", "test"),
           "array('string', 'test')" => array('string', 'test')
);

/* Loop to check for above arrays with var_export() */
echo "\n*** Output for arrays ***\n";
foreach($valid_arrays as $key => $arr) {
	echo "\n--Iteration: $key --\n";
	var_export( $arr );
	echo "\n";
	var_export( $arr, FALSE);
	echo "\n";
	var_dump( var_export( $arr, TRUE) );
	echo "\n";
}
?>
===DONE===
--EXPECT--
*** Testing var_export() with valid arrays ***

*** Output for arrays ***

--Iteration: array() --
array (
)
array (
)
string(9) "array (
)"


--Iteration: array(NULL) --
array (
  0 => NULL,
)
array (
  0 => NULL,
)
string(22) "array (
  0 => NULL,
)"


--Iteration: array(null) --
array (
  0 => NULL,
)
array (
  0 => NULL,
)
string(22) "array (
  0 => NULL,
)"


--Iteration: array(true) --
array (
  0 => true,
)
array (
  0 => true,
)
string(22) "array (
  0 => true,
)"


--Iteration: array("") --
array (
  0 => '',
)
array (
  0 => '',
)
string(20) "array (
  0 => '',
)"


--Iteration: array('') --
array (
  0 => '',
)
array (
  0 => '',
)
string(20) "array (
  0 => '',
)"


--Iteration: array(array(), array()) --
array (
  0 => 
  array (
  ),
  1 => 
  array (
  ),
)
array (
  0 => 
  array (
  ),
  1 => 
  array (
  ),
)
string(55) "array (
  0 => 
  array (
  ),
  1 => 
  array (
  ),
)"


--Iteration: array(array(1, 2), array('a', 'b')) --
array (
  0 => 
  array (
    0 => 1,
    1 => 2,
  ),
  1 => 
  array (
    0 => 'a',
    1 => 'b',
  ),
)
array (
  0 => 
  array (
    0 => 1,
    1 => 2,
  ),
  1 => 
  array (
    0 => 'a',
    1 => 'b',
  ),
)
string(107) "array (
  0 => 
  array (
    0 => 1,
    1 => 2,
  ),
  1 => 
  array (
    0 => 'a',
    1 => 'b',
  ),
)"


--Iteration: array(1 => 'One') --
array (
  1 => 'One',
)
array (
  1 => 'One',
)
string(23) "array (
  1 => 'One',
)"


--Iteration: array("test" => "is_array") --
array (
  'test' => 'is_array',
)
array (
  'test' => 'is_array',
)
string(33) "array (
  'test' => 'is_array',
)"


--Iteration: array(0) --
array (
  0 => 0,
)
array (
  0 => 0,
)
string(19) "array (
  0 => 0,
)"


--Iteration: array(-1) --
array (
  0 => -1,
)
array (
  0 => -1,
)
string(20) "array (
  0 => -1,
)"


--Iteration: array(10.5, 5.6) --
array (
  0 => 10.5,
  1 => 5.6,
)
array (
  0 => 10.5,
  1 => 5.6,
)
string(34) "array (
  0 => 10.5,
  1 => 5.6,
)"


--Iteration: array("string", "test") --
array (
  0 => 'string',
  1 => 'test',
)
array (
  0 => 'string',
  1 => 'test',
)
string(41) "array (
  0 => 'string',
  1 => 'test',
)"


--Iteration: array('string', 'test') --
array (
  0 => 'string',
  1 => 'test',
)
array (
  0 => 'string',
  1 => 'test',
)
string(41) "array (
  0 => 'string',
  1 => 'test',
)"

===DONE===