--TEST--
Bug #77895: IntlDateFormatter::create fails in strict mode if $locale = null 
--EXTENSIONS--
intl
--FILE--
<?php

declare(strict_types=1);

var_dump(IntlDateFormatter::create(null, IntlDateFormatter::NONE, IntlDateFormatter::NONE));

?>
--EXPECT--
object(IntlDateFormatter)#1 (0) {
}
