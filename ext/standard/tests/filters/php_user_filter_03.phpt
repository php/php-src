--TEST--
class php_user_filter#3
--FILE--
<?php
class foo extends php_user_filter {
    function filter($in, $out, &$consumed, $closing): int {}
    function onCreate($var): bool {}
    function onClose(): void {}
}
?>
--EXPECTF--
Fatal error: Declaration of foo::onCreate($var): bool must be compatible with php_user_filter::onCreate(): bool in %s on line %d
