--TEST--
GitHub #12282 IntlDateFormatter::locale with invalid value.
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(new IntlDateFormatter(
	'xx',
	IntlDateFormatter::FULL,
	IntlDateFormatter::FULL,
	null,
	null,
	'w'
));
Locale::setDefault('xx');
var_dump(new IntlDateFormatter(Locale::getDefault()));
--EXPECT--
object(IntlDateFormatter)#1 (0) {
}
object(IntlDateFormatter)#1 (0) {
}
