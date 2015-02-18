--TEST--
Test strict declaration being first operation only 006
--FILE--
<?php
declare(encoding=1);
declare(strict_types=1);

namespace Foo;

var_dump(strlen("abc"));

?>
--EXPECTF--
Warning: declare(encoding=...) ignored because Zend multibyte feature is turned off by settings in %s on line %d

Fatal error: strict_types declaration must be the very first statement in the script in %s on line %d