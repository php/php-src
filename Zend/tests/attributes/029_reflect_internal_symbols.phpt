--TEST--
Reflect attributes on different kinds of internal symbols
--FILE--
<?php

$rf = new ReflectionFunction('unserialize');
var_dump($rf->getAttributes());

$rc = new ReflectionClass('DateTime');
var_dump($rc->getAttributes());

$rm = $rc->getMethod('__construct');
var_dump($rm->getAttributes());

$rcc = $rc->getReflectionConstant('ATOM');
var_dump($rcc->getAttributes());

$rp = new ReflectionProperty('Exception', 'message');
var_dump($rp->getAttributes());

?>
--EXPECT--
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
