--TEST--
Test array_column() function: basic functionality
--FILE--
<?php

echo "*** Testing array_column() : basic functionality ***\n";
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

echo "-- last_name column from recordset, keyed by value from id column --\n";
var_dump(array_column($records, 'last_name', 'id'));

echo "-- last_name column from recordset, keyed by value from first_name column --\n";
var_dump(array_column($records, 'last_name', 'first_name'));

echo "\n*** Testing multiple data types ***\n";
$fh = fopen(__FILE__, 'r', true);
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
var_dump(array_column($values, 'value', 'id'));

echo "\n*** Testing numeric column keys ***\n";
$numericCols = array(
    array('aaa', '111'),
    array('bbb', '222'),
    array('ccc', '333', -1 => 'ddd')
);
var_dump(array_column($numericCols, 1));
var_dump(array_column($numericCols, 1, 0));
var_dump(array_column($numericCols, 1, 0.123));
var_dump(array_column($numericCols, 1, -1));

echo "\n*** Testing failure to find specified column ***\n";
var_dump(array_column($numericCols, 2));
var_dump(array_column($numericCols, 'foo'));
var_dump(array_column($numericCols, 0, 'foo'));
var_dump(array_column($numericCols, 3.14));

echo "\n*** Testing single dimensional array ***\n";
$singleDimension = array('foo', 'bar', 'baz');
var_dump(array_column($singleDimension, 1));

echo "\n*** Testing columns not present in all rows ***\n";
$mismatchedColumns = array(
    array('a' => 'foo', 'b' => 'bar', 'e' => 'bbb'),
    array('a' => 'baz', 'c' => 'qux', 'd' => 'aaa'),
    array('a' => 'eee', 'b' => 'fff', 'e' => 'ggg'),
);
var_dump(array_column($mismatchedColumns, 'c'));
var_dump(array_column($mismatchedColumns, 'c', 'a'));
var_dump(array_column($mismatchedColumns, 'a', 'd'));
var_dump(array_column($mismatchedColumns, 'a', 'e'));
var_dump(array_column($mismatchedColumns, 'b'));
var_dump(array_column($mismatchedColumns, 'b', 'a'));

echo "\n*** Testing use of object converted to string ***\n";
class Foo
{
    public function __toString()
    {
        return 'last_name';
    }
}
class Bar
{
    public function __toString()
    {
        return 'first_name';
    }
}
$f = new Foo();
$b = new Bar();
var_dump(array_column($records, $f));
var_dump(array_column($records, $f, $b));

echo "Done\n";
?>
--EXPECTF--
*** Testing array_column() : basic functionality ***
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
-- last_name column from recordset, keyed by value from id column --
array(3) {
  [1]=>
  string(3) "Doe"
  [2]=>
  string(5) "Smith"
  [3]=>
  string(5) "Jones"
}
-- last_name column from recordset, keyed by value from first_name column --
array(3) {
  ["John"]=>
  string(3) "Doe"
  ["Sally"]=>
  string(5) "Smith"
  ["Jane"]=>
  string(5) "Jones"
}

*** Testing multiple data types ***
array(8) {
  [0]=>
  object(stdClass)#%d (0) {
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
  resource(%d) of type (stream)
}
array(8) {
  [1]=>
  object(stdClass)#%d (0) {
  }
  [2]=>
  float(34.2345)
  [3]=>
  bool(true)
  [4]=>
  bool(false)
  [5]=>
  NULL
  [6]=>
  int(1234)
  [7]=>
  string(3) "Foo"
  [8]=>
  resource(%d) of type (stream)
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
array(3) {
  ["aaa"]=>
  string(3) "111"
  ["bbb"]=>
  string(3) "222"
  ["ccc"]=>
  string(3) "333"
}

Deprecated: Implicit conversion from float 0.123 to int loses precision in %s on line %d
array(3) {
  ["aaa"]=>
  string(3) "111"
  ["bbb"]=>
  string(3) "222"
  ["ccc"]=>
  string(3) "333"
}
array(3) {
  [0]=>
  string(3) "111"
  [1]=>
  string(3) "222"
  ["ddd"]=>
  string(3) "333"
}

*** Testing failure to find specified column ***
array(0) {
}
array(0) {
}
array(3) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(3) "bbb"
  [2]=>
  string(3) "ccc"
}

Deprecated: Implicit conversion from float 3.14 to int loses precision in %s on line %d
array(0) {
}

*** Testing single dimensional array ***
array(0) {
}

*** Testing columns not present in all rows ***
array(1) {
  [0]=>
  string(3) "qux"
}
array(1) {
  ["baz"]=>
  string(3) "qux"
}
array(3) {
  [0]=>
  string(3) "foo"
  ["aaa"]=>
  string(3) "baz"
  [1]=>
  string(3) "eee"
}
array(3) {
  ["bbb"]=>
  string(3) "foo"
  [0]=>
  string(3) "baz"
  ["ggg"]=>
  string(3) "eee"
}
array(2) {
  [0]=>
  string(3) "bar"
  [1]=>
  string(3) "fff"
}
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["eee"]=>
  string(3) "fff"
}

*** Testing use of object converted to string ***
array(3) {
  [0]=>
  string(3) "Doe"
  [1]=>
  string(5) "Smith"
  [2]=>
  string(5) "Jones"
}
array(3) {
  ["John"]=>
  string(3) "Doe"
  ["Sally"]=>
  string(5) "Smith"
  ["Jane"]=>
  string(5) "Jones"
}
Done
