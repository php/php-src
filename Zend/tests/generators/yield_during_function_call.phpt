--TEST--
"yield" can occur during a function call
--FILE--
<?php

function gen() {
    var_dump(str_repeat("x", yield));
}

$gen = gen();
$gen->send(10);

?>
--EXPECT--
string(10) "xxxxxxxxxx"
