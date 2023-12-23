--TEST--
GitHub #12282 IntlDateFormatter::locale with invalid value.
--EXTENSIONS--
intl
--FILE--
<?php

try {
    new IntlDateFormatter(
	    'xyz',
	    IntlDateFormatter::FULL,
	    IntlDateFormatter::FULL,
	    null,
	    null,
	    'w'
    );
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

Locale::setDefault('xx');
try {
    new IntlDateFormatter(Locale::getDefault());
} catch (\ValueError $e) {
    echo $e->getMessage();
}
--EXPECT--
IntlDateFormatter::__construct(): Argument #1 ($locale) "xyz" is invalid
IntlDateFormatter::__construct(): Argument #1 ($locale) "xyz" is invalid
