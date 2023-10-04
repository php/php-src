--TEST--
IntlBreakIterator: clone handler
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(NumberFormatter::ROUND_CEILING);
var_dump(NumberFormatter::ROUND_FLOOR);
var_dump(NumberFormatter::ROUND_DOWN);
var_dump(NumberFormatter::ROUND_TOWARD_ZERO);
var_dump(NumberFormatter::ROUND_UP);
var_dump(NumberFormatter::ROUND_AWAY_FROM_ZERO);
var_dump(NumberFormatter::ROUND_HALFEVEN);
var_dump(NumberFormatter::ROUND_HALFDOWN);
var_dump(NumberFormatter::ROUND_HALFUP);

var_dump(NumberFormatter::ROUND_DOWN === NumberFormatter::ROUND_TOWARD_ZERO);
var_dump(NumberFormatter::ROUND_UP === NumberFormatter::ROUND_AWAY_FROM_ZERO);
?>
--EXPECTF--
int(0)
int(1)

Deprecated: Constant NumberFormatter::ROUND_DOWN is deprecated in %s
int(2)
int(2)

Deprecated: Constant NumberFormatter::ROUND_UP is deprecated in %s
int(3)
int(3)
int(4)
int(5)
int(6)

Deprecated: Constant NumberFormatter::ROUND_DOWN is deprecated in %s
bool(true)

Deprecated: Constant NumberFormatter::ROUND_UP is deprecated in %s
bool(true)