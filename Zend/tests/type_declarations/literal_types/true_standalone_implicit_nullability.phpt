--TEST--
true can be used as a standalone type even with implicit nullability
--FILE--
<?php

function test(true $v = null) {}

?>
===DONE===
--EXPECT--
===DONE===
