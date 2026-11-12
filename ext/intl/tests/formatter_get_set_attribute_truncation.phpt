--TEST--
NumberFormatter::getAttribute()/setAttribute() reject attributes that truncate to a valid one
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die('skip 64-bit only'); ?>
--FILE--
<?php

$fmt = new NumberFormatter("en", NumberFormatter::DECIMAL);

foreach ([NumberFormatter::MAX_FRACTION_DIGITS, NumberFormatter::ROUNDING_INCREMENT] as $attr) {
    $bad = $attr + (1 << 32);

    var_dump($fmt->getAttribute($bad));
    var_dump($fmt->getErrorMessage());
    var_dump(numfmt_get_attribute($fmt, $bad));

    var_dump($fmt->setAttribute($bad, 5));
    var_dump($fmt->getErrorMessage());
    var_dump(numfmt_set_attribute($fmt, $bad, 5));
}

var_dump($fmt->getAttribute(NumberFormatter::MAX_FRACTION_DIGITS));
var_dump($fmt->getAttribute(NumberFormatter::ROUNDING_INCREMENT));

?>
--EXPECT--
bool(false)
string(83) "NumberFormatter::getAttribute(): Error getting attribute value: U_UNSUPPORTED_ERROR"
bool(false)
bool(false)
string(83) "NumberFormatter::setAttribute(): Error setting attribute value: U_UNSUPPORTED_ERROR"
bool(false)
bool(false)
string(83) "NumberFormatter::getAttribute(): Error getting attribute value: U_UNSUPPORTED_ERROR"
bool(false)
bool(false)
string(83) "NumberFormatter::setAttribute(): Error setting attribute value: U_UNSUPPORTED_ERROR"
bool(false)
int(3)
float(0)
