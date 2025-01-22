--TEST--
Observer: Frameless calls are properly observable
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
--FILE--
<?php
function _strpos(string $str) {
    return \strpos($str, 'o', 1);
}
var_dump(_strpos('foo'));
var_dump(_strpos('foo')); // second time, with no observers detected
?>
--EXPECTF--
<!-- init '%s' -->
<!-- init _strpos() -->
<!-- init strpos() -->
<!-- init var_dump() -->
int(1)
int(1)
