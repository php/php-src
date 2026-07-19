--TEST--
Bug #76735 (Incorrect message in fopen on invalid mode)
--FILE--
<?php
try {
	fopen(__FILE__, 'Q');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
ValueError: fopen(): Argument #2 ($mode) must be a valid mode
