--TEST--
yield can be used without a value
--FILE--
<?php

function *recv() {
    while (true) {
        var_dump(yield);
    }
}

$reciever = recv();
var_dump($reciever->current());
$reciever->send(1);
var_dump($reciever->current());
$reciever->send(2);
var_dump($reciever->current());
$reciever->send(3);

?>
--EXPECT--
NULL
int(1)
NULL
int(2)
NULL
int(3)
