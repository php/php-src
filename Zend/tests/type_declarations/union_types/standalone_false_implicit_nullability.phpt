--TEST--
False can be used as a standalone type even with implicit nullability
--FILE--
<?php

function test(false $v = null) {}

?>
===DONE===
--EXPECT--
===DONE===
