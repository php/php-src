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
Fatal error: Declaration of bar::filter($in, $out, &$consumed) must be compatible with php_user_filter::filter($in, $out, &$consumed, bool $closing) in %s on line %d
