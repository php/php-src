--TEST--
Block Pass 002: QM_ASSIGN and INIT_ARRAY
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo($key, $value, array $attributes) {
    return is_array($value)
        ? [$key, array_merge($value, $attributes)]
        : [$value, $attributes];
}
?>
OK
--EXPECT--
OK
