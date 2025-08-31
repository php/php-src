--TEST--
Test strict declaration being first operation only 006
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
--FILE--
<?php
declare(encoding="UTF-8");
declare(strict_types=1);

namespace Foo;

var_dump(strlen("abc"));

?>
--EXPECT--
int(3)
