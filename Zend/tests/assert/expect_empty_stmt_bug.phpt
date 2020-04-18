--TEST--
Empty statement in assert() shouldn't segfault
--FILE--
<?php

assert((function () { return true; })());
echo "ok";

?>
--EXPECT--
ok
