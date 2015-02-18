--TEST--
Test strict declaration being first operation only 007
--SKIPIF--
<?php
if (!in_array("zend.detect_unicode", array_keys(ini_get_all()))) {
  die("skip Requires configure --enable-zend-multibyte option");
}
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
--EXPECTF--
int(3)