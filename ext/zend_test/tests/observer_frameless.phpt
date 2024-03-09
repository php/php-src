--TEST--
Observer: Frameless calls are properly observable
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
--FILE--
<?php
function _strpos(string $str) {
    return \strpos($str, 'o', 1);
}
_strpos('foo');
_strpos('foo'); // second time, with no observers detected
?>
--EXPECTF--
<!-- init '%s' -->
<!-- init _strpos() -->
<!-- init strpos() -->
