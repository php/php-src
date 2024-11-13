--TEST--
Frameless jmp
--FILE--
<?php
namespace Foo;
var_dump(preg_replace("/foo/", '', '', 1));
?>
--EXPECT--
string(0) ""
