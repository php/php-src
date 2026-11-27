--TEST--
ArrayObject::unserialize(): destructors of old elements do not observe the array being released
--FILE--
<?php
class D {
    public function __destruct() {
        global $ao;
        $GLOBALS['keep'] = $ao->getArrayCopy();
    }
}
$ao = new ArrayObject(['pad' => str_repeat('A', 64), 'd' => new D, 'pad2' => str_repeat('B', 64)]);
$ao->unserialize((new ArrayObject([1]))->serialize());
var_dump($keep);
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
