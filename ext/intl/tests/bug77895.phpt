--TEST--
Bug #77895: IntlDateFormatter::create fails in strict mode if $locale = null 
--SKIPIF--
<?php if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
--FILE--
<?php

declare(strict_types=1);

var_dump(IntlDateFormatter::create(null, IntlDateFormatter::NONE, IntlDateFormatter::NONE));

?>
--EXPECT--
object(IntlDateFormatter)#1 (0) {
}
