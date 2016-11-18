--TEST--
Bug #69732 (can induce segmentation fault with basic php code)
--FILE--
<?php
class wpq {
    private $unreferenced;
 
    public function __get($name) {
        return $this->$name . "XXX";
    }
}
 
function ret_assoc() {
	$x = "XXX";
    return array('foo' => 'bar', $x);
}
 
$wpq = new wpq;
$wpq->interesting =& ret_assoc();
$x = &$wpq->interesting;
var_dump($x);
?>
--EXPECTF--
Notice: Only variables should be assigned by reference in %s on line %d
array(2) {
  ["foo"]=>
  string(3) "bar"
  [0]=>
  string(3) "XXX"
}
