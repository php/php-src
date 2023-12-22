--TEST--
GitHub #12943 IntlDateFormatter::locale accepting "C".
--EXTENSIONS--
intl
--FILE--
<?php

$fmt = new IntlDateFormatter(
        'C',
        IntlDateFormatter::FULL,
        IntlDateFormatter::FULL,
        );
var_dump($fmt->getLocale(ULOC_VALID_LOCALE));
--EXPECTF--
string(%d) "%s"
