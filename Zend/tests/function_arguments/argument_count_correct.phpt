--TEST--
Call function with correct number of arguments
--FILE--
<?php
function foo() { }
foo();

function bar($foo, $bar) { }
bar(1, 2);

function bat(int $foo, string $bar) { }
bat(123, "foo");
bat("123", "foo");

$fp = fopen(__FILE__, "r");
fclose($fp);

echo "done";
--EXPECT--
done
