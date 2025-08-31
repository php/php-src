--TEST--
IntlBreakIterator::__construct() should not be callable
--EXTENSIONS--
intl
--FILE--
<?php

try {
	new IntlBreakIterator();
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Call to private IntlBreakIterator::__construct() from global scope
