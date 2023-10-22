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
} catch (\IntlException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
datefmt_create: invalid locale: U_ILLEGAL_ARGUMENT_ERROR
