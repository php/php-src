--TEST--
Bug #68775: yield in a function argument crashes or loops indefinitely
--FILE--
<?php

function a($x) {
    var_dump($x);
}

function gen() {
     a(yield);
}

$g = gen();
$g->send(1);

?>
--EXPECT--
int(1)
