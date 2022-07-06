--TEST--
Bug #31141 (properties declared in the class extending MySQLi are not available)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class Test extends mysqli
{
    public $test = array();

    function foo()
    {
        $ar_test = array("foo", "bar");
        $this->test = &$ar_test;
    }
}

$my_test = new Test;
$my_test->foo();
var_dump($my_test->test);
?>
--EXPECTF--
array(2) {
  [0]=>
  %s(3) "foo"
  [1]=>
  %s(3) "bar"
}
