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
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
array(0) {
}
array(0) {
}
object(Generator)#1 (0) {
}
object(Generator)#1 (0) {
}
==DONE==
