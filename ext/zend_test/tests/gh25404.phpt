--TEST--
GH-25404 (zend_call_method() forwards by-reference returns)
--EXTENSIONS--
zend_test
--FILE--
<?php

class Demo {
    public mixed $v = true;
    public function &returnsRef() { return $this->v; }
}

$d = new Demo();
var_dump(zend_call_method($d, 'returnsRef'));

?>
--EXPECT--
bool(true)
