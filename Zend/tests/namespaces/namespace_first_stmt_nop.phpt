--TEST--
Nop statement before namespace
--FILE--
<?php
;
namespace Foo;
?>
===DONE===
--EXPECT--
===DONE===
