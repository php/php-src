--TEST--
Call userland function with incorrect number of arguments with strict types
--FILE--
<?php
declare(strict_types=1);
try {
	function foo($bar) { }
	foo();
} catch (\Error $e) {
	echo get_class($e) . PHP_EOL;
	echo $e->getMessage() . PHP_EOL;
}

try {
	function bar($foo, $bar) { }
	bar(1);
} catch (\Error $e) {
	echo get_class($e) . PHP_EOL;
	echo $e->getMessage() . PHP_EOL;
}

function bat(int $foo, string $bar) { }

try {
	bat(123);
} catch (\Error $e) {
	echo get_class($e) . PHP_EOL;
	echo $e->getMessage() . PHP_EOL;
}

try {
	bat("123");
} catch (\Error $e) {
	echo get_class($e) . PHP_EOL;
	echo $e->getMessage() . PHP_EOL;
}

try {
	bat(123, 456);
} catch (\Error $e) {
	echo get_class($e) . PHP_EOL;
	echo $e->getMessage() . PHP_EOL;
}
--EXPECTF--
ArgumentCountError
Too few arguments to function foo(), 0 passed in %s and exactly 1 expected
ArgumentCountError
Too few arguments to function bar(), 1 passed in %s and exactly 2 expected
ArgumentCountError
Too few arguments to function bat(), 1 passed in %s and exactly 2 expected
TypeError
Argument 1 passed to bat() must be of the type integer, string given, called in %s
TypeError
Argument 2 passed to bat() must be of the type string, integer given, called in %s
