--TEST--
Test that legacy IS_ITERABLE arg info type generates a notice
--EXTENSIONS--
zend_test
--FILE--
<?php

function gen() {
    yield 1;
};

var_dump(zend_iterable_legacy([], null));
var_dump(zend_iterable_legacy([], []));
var_dump(zend_iterable_legacy(gen(), null));
var_dump(zend_iterable_legacy(gen(), gen()));

?>
==DONE==
--EXPECT--
array(0) {
}
array(0) {
}
object(Generator)#1 (0) {
}
object(Generator)#1 (0) {
}
==DONE==
