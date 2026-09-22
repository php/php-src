--TEST--
Bug #71572: String offset assignment from an empty string inserts null byte
--FILE--
<?php

$str = "abc";
try {
    var_dump($str[0] = "");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($str[1] = "");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($str[3] = "");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($str[10] = "");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($str);
?>
--EXPECT--
Error: Cannot assign an empty string to a string offset
Error: Cannot assign an empty string to a string offset
Error: Cannot assign an empty string to a string offset
Error: Cannot assign an empty string to a string offset
string(3) "abc"
