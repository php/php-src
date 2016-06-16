--TEST--
Trying to redeclare constant inside namespace
--FILE--
<?php

namespace foo;

const foo = 1;
try {
	define("foo\\foo", 2);
} catch (\Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Exception: Constant foo\foo already defined
