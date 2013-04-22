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

/* Array from Bug Request #64493 test script */
$rows = array(
	456 => array('id' => '3', 'title' => 'Foo', 'date' => '2013-03-25'),
	457 => array('id' => '5', 'title' => 'Bar', 'date' => '2012-05-20'),
);

echo "-- pass null as second parameter to get back all columns indexed by third parameter --\n";
var_dump(array_column($rows, null, 'id'));

echo "-- pass null as second parameter and bogus third param to get back zero-indexed array of all columns --\n";
var_dump(array_column($rows, null, 'foo'));

echo "-- pass null as second parameter and no third param to get back the exact same array as the input --\n";
var_dump(array_column($rows, null));

echo "-- pass array as second parameter of column names to retrieve, specifying third parameter --\n";
var_dump(array_column($rows, array('title', 'date'), 'id'));

echo "-- pass array as second parameter of column names to retrieve, specifying bogus third parameter --\n";
var_dump(array_column($rows, array('id', 'title'), 'foo'));

echo "-- pass array as second parameter of column names to retrieve, with no third parameter --\n";
var_dump(array_column($rows, array('title', 'date')));


$rows2 = array(
	456 => array('id' => '3', 'title' => 'Foo', 'date' => '2013-03-25', 0 => 'John'),
	457 => array('id' => '5', 'title' => 'Bar', 'date' => '2012-05-20', 0 => 'Sally'),
);

echo "-- test second paramater as array with mixed column names --\n";
var_dump(array_column($rows2, array('title', 0), 'id'));

echo "-- test second parameter as array with bogus column names --\n";
var_dump(array_column($rows2, array('foo', 'bar'), 'id'));

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
-- pass null as second parameter to get back all columns indexed by third parameter --
array(2) {
  [3]=>
  array(3) {
    ["id"]=>
    string(1) "3"
    ["title"]=>
    string(3) "Foo"
    ["date"]=>
    string(10) "2013-03-25"
  }
  [5]=>
  array(3) {
    ["id"]=>
    string(1) "5"
    ["title"]=>
    string(3) "Bar"
    ["date"]=>
    string(10) "2012-05-20"
  }
}
-- pass null as second parameter and bogus third param to get back zero-indexed array of all columns --
array(2) {
  [0]=>
  array(3) {
    ["id"]=>
    string(1) "3"
    ["title"]=>
    string(3) "Foo"
    ["date"]=>
    string(10) "2013-03-25"
  }
  [1]=>
  array(3) {
    ["id"]=>
    string(1) "5"
    ["title"]=>
    string(3) "Bar"
    ["date"]=>
    string(10) "2012-05-20"
  }
}
-- pass null as second parameter and no third param to get back the exact same array as the input --
array(2) {
  [456]=>
  array(3) {
    ["id"]=>
    string(1) "3"
    ["title"]=>
    string(3) "Foo"
    ["date"]=>
    string(10) "2013-03-25"
  }
  [457]=>
  array(3) {
    ["id"]=>
    string(1) "5"
    ["title"]=>
    string(3) "Bar"
    ["date"]=>
    string(10) "2012-05-20"
  }
}
-- pass array as second parameter of column names to retrieve, specifying third parameter --
array(2) {
  [3]=>
  array(2) {
    ["title"]=>
    string(3) "Foo"
    ["date"]=>
    string(10) "2013-03-25"
  }
  [5]=>
  array(2) {
    ["title"]=>
    string(3) "Bar"
    ["date"]=>
    string(10) "2012-05-20"
  }
}
-- pass array as second parameter of column names to retrieve, specifying bogus third parameter --
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "3"
    ["title"]=>
    string(3) "Foo"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "5"
    ["title"]=>
    string(3) "Bar"
  }
}
-- pass array as second parameter of column names to retrieve, with no third parameter --
array(2) {
  [456]=>
  array(2) {
    ["title"]=>
    string(3) "Foo"
    ["date"]=>
    string(10) "2013-03-25"
  }
  [457]=>
  array(2) {
    ["title"]=>
    string(3) "Bar"
    ["date"]=>
    string(10) "2012-05-20"
  }
}
-- test second paramater as array with mixed column names --
array(2) {
  [3]=>
  array(2) {
    ["title"]=>
    string(3) "Foo"
    [0]=>
    string(4) "John"
  }
  [5]=>
  array(2) {
    ["title"]=>
    string(3) "Bar"
    [0]=>
    string(5) "Sally"
  }
}
-- test second parameter as array with bogus column names --
array(2) {
  [3]=>
  array(0) {
  }
  [5]=>
  array(0) {
  }
}
Done
