--TEST--
JIT ZEND_MATCH with reference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=1205
--SKIPIF--
<?php require_once(__DIR__ . '/../skipif.inc'); ?>
--FILE--
<?php

function test(string|int $value) {
    $reference = &$value;
    var_dump(match ($reference) {
        1, 2, 3, 4, 5 => 'number',
        '1', '2', '3', '4', '5' => 'string',
        default => 'bar',
    });
}

test(3);
test('3');
test(42);
test('foo');

?>
--EXPECT--
string(6) "number"
string(6) "string"
string(3) "bar"
string(3) "bar"
