--TEST--
PRE_INC/DEC numeric string
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
function test($b) {
    $a = "0";
    $i = 0;
    while (is_numeric($a)) {
        $a .= $b;
        $a--;
        $i .= $a;
        $i++;
    }
    var_dump($a, $i);
}
test("0");
?>
--EXPECTF--
Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d

Deprecated: Decrement on non-numeric string has no effect and is deprecated in %s on line %d

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(5) "-INF0"
string(260) "0-2-12-112-1112-11112-111112-1111112-11111112-111111112-1111111112-11111111112-111111111112-1111111111112-11111111111112-111111111111112-1111111111111112-11111111111111112-111111111111111112-1111111111111111112-1.1111111111111E+20-1.1111111111111E+191-ING-INF1"
