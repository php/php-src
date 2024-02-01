--TEST--
Bug #39576 (array_walk() doesn't separate userdata zval)
--FILE--
<?php

class Test {
    public $_table = '';
    public $_columns = array ();
    public $_primary = array ();
    public $name;

}

$test = new Test ();
$test->name = 'test';
$test->_columns['name'] = new stdClass;

function test ($value, $column, &$columns) {}

array_walk (
    get_object_vars ($test),
    'test',
    $test->_columns
);

var_dump($test);

array_intersect_key (
    get_object_vars ($test),
    $test->_primary
);

echo "Done\n";
?>
--EXPECTF--
Notice: Only variables should be passed by reference in %s on line %d

Warning: test(): Argument #3 ($columns) must be passed by reference, value given in %s on line %d

Warning: test(): Argument #3 ($columns) must be passed by reference, value given in %s on line %d

Warning: test(): Argument #3 ($columns) must be passed by reference, value given in %s on line %d

Warning: test(): Argument #3 ($columns) must be passed by reference, value given in %s on line %d
object(Test)#%d (4) {
  ["_table"]=>
  string(0) ""
  ["_columns"]=>
  array(1) {
    ["name"]=>
    object(stdClass)#%d (0) {
    }
  }
  ["_primary"]=>
  array(0) {
  }
  ["name"]=>
  string(4) "test"
}
Done
