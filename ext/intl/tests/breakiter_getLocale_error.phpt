--TEST--
IntlBreakIterator::getLocale(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php

$bi = IntlBreakIterator::createSentenceInstance('pt');

try {
	$bi->getLocale(2);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	$bi->getLocale(-1);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: IntlBreakIterator::getLocale(): Argument #1 ($type) must be either Locale::ACTUAL_LOCALE or Locale::VALID_LOCALE
ValueError: IntlBreakIterator::getLocale(): Argument #1 ($type) must be either Locale::ACTUAL_LOCALE or Locale::VALID_LOCALE
