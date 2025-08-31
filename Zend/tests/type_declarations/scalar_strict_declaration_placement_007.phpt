--TEST--
Test strict declaration being first operation only 007
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
--FILE--
<?php
declare(strict_types=1);
declare(encoding="ISO-8859-1");

namespace Foo;

var_dump(strlen("abc"));

?>
--EXPECT--
int(3)
