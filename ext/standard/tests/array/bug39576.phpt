--TEST--
Bug #39576 (array_walk() doesn't separate userdata zval)
--FILE--
<?php

class Test {

	public $_table = '';
	public $_columns = array ();
	public $_primary = array ();

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
Strict Standards: Only variables should be passed by reference in %s on line %d
object(Test)#%d (4) {
  [u"_table"]=>
  unicode(0) ""
  [u"_columns"]=>
  array(1) {
    [u"name"]=>
    object(stdClass)#%d (0) {
    }
  }
  [u"_primary"]=>
  array(0) {
  }
  [u"name"]=>
  unicode(4) "test"
}
Done
