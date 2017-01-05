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
Warning: Declaration of foo::onCreate($var) should be compatible with php_user_filter::onCreate() in %s on line %d
