--TEST--
Bug #77310 (1): Incorrect SCCP for compound assign to arrays
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function breakit($data_arr) {
    $foo[0] = "";
    for ($i = 0; $i < count($data_arr); $i++) {
        $foo[0] .= $data_arr[$i];
    }
    echo $foo[0] . "\n";
}

$data = ['zero', 'one', 'two'];
breakit($data);

?>
--EXPECT--
zeroonetwo
