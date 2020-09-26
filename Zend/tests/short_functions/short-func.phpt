--TEST--

--FILE--
<?php

function test(int $a) => $a + 1;

print test(5);

?>
--EXPECT--
6
