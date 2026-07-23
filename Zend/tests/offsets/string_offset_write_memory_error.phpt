--TEST--
Writing to a string at an offset too large for the memory limit throws a catchable MemoryError
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$s = 'abc';
try {
    $s[PHP_INT_MAX] = 'Z';
} catch (MemoryError $e) {
    echo $e->getMessage() . "\n";
}
var_dump($s);

// The throw unwinds the whole assignment, so the outer target keeps its previous value.
$r = 'untouched';
try {
    $r = ($s[PHP_INT_MAX] = 'Z');
} catch (MemoryError $e) {
    echo "caught\n";
}
var_dump($r);
var_dump($s);

?>
--EXPECT--
String offset assignment produces a string too large to fit in the configured memory limit
string(3) "abc"
caught
string(9) "untouched"
string(3) "abc"
