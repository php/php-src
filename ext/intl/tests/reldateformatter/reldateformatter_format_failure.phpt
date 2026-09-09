--TEST--
IntlRelativeDateTimeFormatter returns false on number formatting failure
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '62.1') < 0) die('skip for ICU >= 62.1'); ?>
--FILE--
<?php

function createFailingFormatter(): IntlRelativeDateTimeFormatter
{
    $numberFormatter = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
    $numberFormatter->setAttribute(NumberFormatter::ROUNDING_INCREMENT, NAN);

    return new IntlRelativeDateTimeFormatter('en_US', numberFormatter: $numberFormatter);
}

foreach (['format', 'formatNumeric'] as $method) {
    $formatter = createFailingFormatter();

    var_dump($formatter->$method(1, IntlRelativeDateTimeFormatter::UNIT_SECOND));
    var_dump($formatter->getErrorCode() !== U_ZERO_ERROR);
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
