--TEST--
IntlRelativeDateTimeFormatter returns false on number formatting failure
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '62.1') < 0) die('skip requires ICU >= 62.1 for number formatting errors'); ?>
--FILE--
<?php

function createFailingFormatter(): IntlRelativeDateTimeFormatter
{
    $numberFormatter = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
    $numberFormatter->setAttribute(NumberFormatter::MAX_FRACTION_DIGITS, 0);
    // ICU's UNUM_ROUND_UNNECESSARY is not exposed as a NumberFormatter constant.
    $numberFormatter->setAttribute(NumberFormatter::ROUNDING_MODE, 7);

    return new IntlRelativeDateTimeFormatter('en_US', numberFormatter: $numberFormatter);
}

foreach (['format', 'formatNumeric'] as $method) {
    $formatter = createFailingFormatter();

    var_dump($formatter->$method(1.5, IntlRelativeDateTimeFormatterUnit::Second));
    var_dump(intl_error_name($formatter->getErrorCode()) === 'U_FORMAT_INEXACT_ERROR');
    var_dump(intl_get_error_code() === $formatter->getErrorCode());
}

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
