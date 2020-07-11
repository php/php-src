--TEST--
Incorrect negative range inference
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function test() {
    for ($i = 0; $i < 10; $i++) {
        if ($i != 5) {
            $t = (int) ($i < 5);
            var_dump($t);
        }
    }
}
test();

?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
int(1)
int(0)
int(0)
int(0)
int(0)
