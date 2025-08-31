--TEST--
class php_user_filter#1
--FILE--
<?php
class foo extends php_user_filter {
    function filter($in, $out, &$consumed, $closing): int {}
    function onCreate(): bool {}
    function onClose(): void {}
}
class bar extends php_user_filter {
    function filter($in, $out, &$consumed): int {}
    function onCreate(): bool {}
    function onClose(): void {}
}
?>
--EXPECTF--
Fatal error: Declaration of bar::filter($in, $out, &$consumed): int must be compatible with php_user_filter::filter($in, $out, &$consumed, bool $closing): int in %s on line %d
