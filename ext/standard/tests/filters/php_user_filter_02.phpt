--TEST--
class php_user_filter#2
--FILE--
<?php
class foo extends php_user_filter {
    function filter($in, $out, $consumed, $closing): int {}
    function onCreate(): bool {}
    function onClose(): void {}
}
?>
--EXPECTF--
Fatal error: Declaration of foo::filter($in, $out, $consumed, $closing): int must be compatible with php_user_filter::filter($in, $out, &$consumed, bool $closing): int in %s on line %d
