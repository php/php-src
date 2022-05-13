--TEST--
There shouldn't be any leaks when the generator's return value isn't used
--FILE--
<?php

function gen($foo) { yield; }

gen('foo'); // return value not used

?>
===DONE===
--EXPECT--
===DONE===
