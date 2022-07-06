--TEST--
class php_user_filter#2
--FILE--
<?php
class foo extends php_user_filter {
    function filter($in, $out, $consumed, $closing) {}
    function onCreate() {}
    function onClose() {}
}
?>
--EXPECTF--
Fatal error: Declaration of foo::filter($in, $out, $consumed, $closing) must be compatible with php_user_filter::filter($in, $out, &$consumed, bool $closing) in %s on line %d
