--TEST--
Null and false can be used in a union type
--FILE--
<?php

function test1(): null|false {}
function test2(): false|null {}

?>
===DONE===
--EXPECT--
===DONE===
