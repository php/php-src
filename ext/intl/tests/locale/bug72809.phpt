--TEST--
Bug #72809 (Locale::lookup() wrong result with canonicalize option)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die("skip intl extension not avaible");
?>
--FILE--
<?php
var_dump(
    Locale::lookup(['en', 'en-US'], 'en-US-u-cu-EUR-tz-deber-fw-mon', true),
    Locale::lookup(['en', 'en_US'], 'en_US@currency=eur;fw=mon;timezone=Europe/Berlin', false),
    Locale::lookup(['en', 'en_US'], 'en_US@currency=eur;fw=mon;timezone=Europe/Berlin', true),
);
?>
--EXPECT--
string(5) "en_us"
string(5) "en_US"
string(5) "en_us"
