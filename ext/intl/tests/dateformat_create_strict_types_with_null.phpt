--TEST--
Creating IntlDateFormatter in strict mode with $locale, $dateType and $timeType set to null
--FILE--
<?php

declare(strict_types=1);

var_dump(IntlDateFormatter::create(null, null, null));

?>
--EXPECT--
object(IntlDateFormatter)#1 (0) {
}
