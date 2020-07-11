--TEST--
Incorrect optimization of $i = $i++
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function test() {
    $i = 1;
    $i = $i++;
    var_dump($i);

    $i = 1;
    $i = $i--;
    var_dump($i);
}
test();

?>
--EXPECT--
int(1)
int(1)
