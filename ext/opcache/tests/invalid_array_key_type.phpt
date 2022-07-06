--TEST--
Don't add array value type is key type is illegal
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function test(\SplObjectStorage $definitions = null) {
    $argument = new stdClass;
    $definitions[$argument] = 1;
    $definitions[$argument] += 1;
    $argument = [];
    $definitions[$argument] = 1;
    $definitions[$argument] += 1;
}
function test2() {
    $a[[]] = $undef;
}

?>
===DONE===
--EXPECT--
===DONE===
