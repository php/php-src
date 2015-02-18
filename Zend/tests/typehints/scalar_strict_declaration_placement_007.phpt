--TEST--
Test strict declaration being first operation only 007
--FILE--
<?php
declare(strict_types=1);
declare(encoding=1);

namespace Foo;

var_dump(strlen("abc"));

?>
--EXPECTF--
Warning: declare(encoding=...) ignored because Zend multibyte feature is turned off by settings in %s on line %d
int(3)