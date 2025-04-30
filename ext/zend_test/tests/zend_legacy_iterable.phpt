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
--EXPECTF--
array(0) {
}
array(0) {
}
object(Generator)#%d (1) {
  ["function"]=>
  string(3) "gen"
}
object(Generator)#%d (1) {
  ["function"]=>
  string(3) "gen"
}
==DONE==
