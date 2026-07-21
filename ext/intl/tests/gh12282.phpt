--TEST--
GitHub #12282 IntlDateFormatter::locale with invalid value.
--EXTENSIONS--
intl
--FILE--
<?php

try {
    new IntlDateFormatter(
	    'xx',
	    IntlDateFormatter::FULL,
	    IntlDateFormatter::FULL,
	    null,
	    null,
	    'w'
    );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

Locale::setDefault('xx');
try {
    new IntlDateFormatter(Locale::getDefault());
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
--EXPECT--
ValueError: IntlDateFormatter::__construct(): Argument #1 ($locale) "xx" is invalid
ValueError: IntlDateFormatter::__construct(): Argument #1 ($locale) "xx" is invalid
