--TEST--
__HALT_COMPILER(); bad define() of __COMPILER_HALT_OFFSET__ 1
--FILE--
<?php
try {
	define ('__COMPILER_HALT_OFFSET__', 1);
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
?>
==DONE==
--EXPECT--
Exception: Constant __COMPILER_HALT_OFFSET__ already defined
==DONE==