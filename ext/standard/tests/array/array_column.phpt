--TEST--
Test array_column_values() function
--FILE--
<?php
/* Prototype:
 *  array array_column(array $input, mixed $key);
 * Description:
 *  Returns an array containing all the values from
 *  the specified "column" in a two-dimensional array.
 */

echo "*** Testing basic functionalities ***\n";
/* Array representing a possible record set returned from a database */
$records = array(
	array(
		'id' => 1,
		'first_name' => 'John',
		'last_name' => 'Doe'
	),
	array(
		'id' => 2,
		'first_name' => 'Sally',
		'last_name' => 'Smith'
	),
	array(
		'id' => 3,
		'first_name' => 'Jane',
		'last_name' => 'Jones'
	)
);

echo "-- first_name column from recordset --\n";
var_dump(array_column($records, 'first_name'));

echo "-- id column from recordset --\n";
var_dump(array_column($records, 'id'));

echo "\n*** Testing multiple data types ***\n";
$file = basename(__FILE__);
$fh = fopen($file, 'r', true);
$values = array(
	array(
		'id' => 1,
		'value' => new stdClass
	),
	array(
		'id' => 2,
		'value' => 34.2345
	),
	array(
		'id' => 3,
		'value' => true
	),
	array(
		'id' => 4,
		'value' => false
	),
	array(
		'id' => 5,
		'value' => null
	),
	array(
		'id' => 6,
		'value' => 1234
	),
	array(
		'id' => 7,
		'value' => 'Foo'
	),
	array(
		'id' => 8,
		'value' => $fh
	)
);
var_dump(array_column($values, 'value'));

echo "\n*** Testing numeric column keys ***\n";
$numericCols = array(
	array('aaa', '111'),
	array('bbb', '222'),
	array('ccc', '333')
);
var_dump(array_column($numericCols, 1));

echo "Done\n";
?>
--EXPECTF--
*** Testing basic functionalities ***
-- first_name column from recordset --
array(3) {
  [0]=>
  string(4) "John"
  [1]=>
  string(5) "Sally"
  [2]=>
  string(4) "Jane"
}
-- id column from recordset --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

*** Testing multiple data types ***
array(8) {
  [0]=>
  object(stdClass)#1 (0) {
  }
  [1]=>
  float(34.2345)
  [2]=>
  bool(true)
  [3]=>
  bool(false)
  [4]=>
  NULL
  [5]=>
  int(1234)
  [6]=>
  string(3) "Foo"
  [7]=>
  resource(5) of type (stream)
}

*** Testing numeric column keys ***
array(3) {
  [0]=>
  string(3) "111"
  [1]=>
  string(3) "222"
  [2]=>
  string(3) "333"
}
Done
