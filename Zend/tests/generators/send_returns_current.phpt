--TEST--
$generator->send() returns the yielded value
--FILE--
<?php

function reverseEchoGenerator() {
    $data = yield;
    while (true) {
        $data = (yield strrev($data));
    }
}

$gen = reverseEchoGenerator();
var_dump($gen->send('foo'));
var_dump($gen->send('bar'));

?>
--EXPECT--
string(3) "oof"
string(3) "rab"
