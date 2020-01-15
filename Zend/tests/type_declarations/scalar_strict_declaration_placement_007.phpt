--TEST--
Test strict declaration being first operation only 007
--SKIPIF--
<?php
if (!extension_loaded("mbstring")) {
  die("skip Requires mbstring extension");
}
?>
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
