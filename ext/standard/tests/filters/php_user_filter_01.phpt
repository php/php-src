--TEST--
class php_user_filter#1
--FILE--
<?php
class foo extends php_user_filter {
    function filter($in, $out, &$consumed, $closing) {}
    function onCreate() {}
    function onClose() {}
}
class bar extends php_user_filter {
    function filter($in, $out, &$consumed) {}
    function onCreate() {}
    function onClose() {}
}
?>
--EXPECTF--
Strict Standards: Declaration of bar::filter() should be compatible with php_user_filter::filter($in, $out, &$consumed, $closing) in %s on line %d
