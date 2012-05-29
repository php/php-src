--TEST--
Calls to send() after close should do nothing
--FILE--
<?php

function *gen() { }

$gen = gen();
$gen->send("Test");

?>
===DONE===
--EXPECT--
===DONE===
