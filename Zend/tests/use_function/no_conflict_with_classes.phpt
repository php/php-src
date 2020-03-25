--TEST--
"use function" should not conflict with class names
--FILE--
<?php

namespace Foo;

class Bar {}

use function bar;

?>
===DONE===
--EXPECT--
===DONE===
