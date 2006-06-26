--TEST--
class php_user_filter#3
--FILE--
<?php
class foo extends php_user_filter {
    function filter($in, $out, &$consumed, $closing) {}
    function onCreate($var) {}
    function onClose() {}
}
?>
--EXPECTF--
Strict Standards: Declaration of foo::onCreate() should be compatible with that of php_user_filter::onCreate() in %s on line %d
