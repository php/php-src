--TEST--
Block Pass 002: QM_ASSIGN and INIT_ARRAY
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
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
