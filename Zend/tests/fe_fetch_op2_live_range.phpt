--TEST--
FE_FETCH op2 is a def and needs special live range handling
--INI--
opcache.enable_cli=1
--FILE--
<?php
try {
    foreach (["test"] as $k => func()[]) {}
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// The key is an int, the value is a refcounted string: the value's live range
// must not be dropped based on the type of the key.
function test($s) {
    foreach ([$s . $s] as $k => func()[]) {}
}
try {
    test("test");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Call to undefined function func()
Call to undefined function func()
