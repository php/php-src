--TEST--
Test array_column() function: variant functionality
--FILE--
<?php
echo "*** Testing array_column() : variant functionality ***\n";
/* Array representing a possible record set returned from a database */
$records = array(
	456 => array(
		'id' => 1,
		'first_name' => 'John',
		'last_name' => 'Doe'
	),
	457 => array(
		'id' => 2,
		'first_name' => 'Sally',
		'last_name' => 'Smith'
	),
	458 => array(
		'id' => 3,
		'first_name' => 'Jane',
		'last_name' => 'Jones'
	)
);

echo "-- first_name column from recordset --\n";
var_dump(array_column($records, 'first_name'));

echo "-- first_name column from recordset, keyed by value from id column --\n";
var_dump(array_column($records, 'first_name', 'id'));

echo "-- first_name column from recordset, zero-indexed due to missing index column --\n";
var_dump(array_column($records, 'first_name', 'foo'));

echo "Done\n";
?>
--EXPECTF--
*** Testing array_column() : variant functionality ***
-- first_name column from recordset --
array(3) {
  [456]=>
  string(4) "John"
  [457]=>
  string(5) "Sally"
  [458]=>
  string(4) "Jane"
}
-- first_name column from recordset, keyed by value from id column --
array(3) {
  [1]=>
  string(4) "John"
  [2]=>
  string(5) "Sally"
  [3]=>
  string(4) "Jane"
}
-- first_name column from recordset, zero-indexed due to missing index column --
array(3) {
  [0]=>
  string(4) "John"
  [1]=>
  string(5) "Sally"
  [2]=>
  string(4) "Jane"
}
Done
