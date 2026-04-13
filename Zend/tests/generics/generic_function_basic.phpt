--TEST--
Generic function: basic declaration and invocation
--FILE--
<?php
function identity<T>(T $value): T {
    return $value;
}

echo identity(42) . "\n";
echo identity("hello") . "\n";
echo identity(3.14) . "\n";
?>
--EXPECT--
42
hello
3.14
