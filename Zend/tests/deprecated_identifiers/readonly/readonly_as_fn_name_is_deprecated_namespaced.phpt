--TEST--
Naming a function readonly is deprecated
--FILE--
<?php

namespace Foo;

function readonly() {}

?>
DONE
--EXPECTF--
Deprecated: Calling a function "readonly" is deprecated since 8.6 in %s on line %d
DONE
