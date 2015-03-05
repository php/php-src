--TEST--
Bug #42859 (import always conflicts with internal classes)
--FILE--
<?php
namespace Foo;
use Blah\Exception;

var_dump(Exception::class);
?>
--EXPECTF--
string(14) "Blah\Exception"
