--TEST--
Parameter type hint - formatting
--FILE--
<?php

function test(int$foo) { print "ok\n";}
test(1);

function test2(int


$foo) { print "ok\n";}
test2(1);

?>
--EXPECT--
ok
ok
