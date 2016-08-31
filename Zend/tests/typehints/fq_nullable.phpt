--TEST--
Fully-qualified nullable parameter type
--FILE--
<?php

namespace Foo;
function test(?\stdClass $param) {}
test(new \stdClass);

?>
===DONE===
--EXPECT--
===DONE===
