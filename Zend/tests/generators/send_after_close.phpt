--TEST--
Calls to send() after close should do nothing
--FILE--
<?php

function gen() { var_dump(yield); }

$gen = gen();
$gen->send('foo');
$gen->send('bar');

?>
--EXPECT--
string(3) "foo"
