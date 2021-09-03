--TEST--
Bug #68471 (IntlDateFormatter fails for "GMT+00:00" timezone)
--EXTENSIONS--
intl
--FILE--
<?php
$formatter = new IntlDateFormatter(
    'fr_FR',
    IntlDateFormatter::NONE,
    IntlDateFormatter::NONE,
    "GMT+00:00"
);
var_dump($formatter);
?>
--EXPECT--
object(IntlDateFormatter)#1 (0) {
}
