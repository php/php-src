--TEST--
Bug #43505 (Assign by reference bug)
--INI--
error_reporting=0
--FILE--
<?php
class Test implements Countable {
    public function count() {
        return $some;
    }
}

$obj = new Test();

$a = array();
$b =& $a['test'];
var_dump($a);

$t = count($obj);

$a = array();
$b =& $a['test'];
var_dump($a);
?>
--EXPECT--
array(1) {
  ["test"]=>
  &NULL
}
array(1) {
  ["test"]=>
  &NULL
}
