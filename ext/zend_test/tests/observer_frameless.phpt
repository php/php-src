--TEST--
Observer: Frameless calls are disabled when there are observers
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
?>
--EXPECTF--
<!-- init '%s' -->
<!-- init _strpos() -->
<!-- init strpos() -->
