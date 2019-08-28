--TEST--
Fully-qualified nullable parameter type
--FILE--
<?php

namespace Foo;
function test(?\StdClass $param) {}
test(new \StdClass);

?>
===DONE===
--EXPECT--
===DONE===
