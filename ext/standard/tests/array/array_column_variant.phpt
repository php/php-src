--TEST--
Test array_column() function: variant functionality
--FILE--
<?php
/* Array from Bug Request #64493 test script */
$rows = array(
  456 => array('id' => '3', 'title' => 'Foo', 'date' => '2013-03-25'),
  457 => array('id' => '5', 'title' => 'Bar', 'date' => '2012-05-20'),
);

echo "-- pass null as second parameter to get back all columns indexed by third parameter --\n";
var_dump(array_column($rows, null, 'id'));

echo "-- pass null as second parameter and bogus third param to get back zero-indexed array of all columns --\n";
var_dump(array_column($rows, null, 'foo'));

echo "-- pass null as second parameter and no third param to get back array_values(input) --\n";
var_dump(array_column($rows, null));

echo "Done\n";
--EXPECTF--
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
-- pass null as second parameter and no third param to get back array_values(input) --
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
Done
