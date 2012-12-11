--TEST--
Rise exception in the generator
--FILE--
<?php

function gen() {
    var_dump("start generator");
    try {
        var_dump(yield "yield foo");
        var_dump("throws failed");
    } catch(Exception $e) {
        var_dump($e->getMessage());
    }
    var_dump(yield "yield bar");
}

$gen = gen();
var_dump($gen->current());
$gen->rise(new Exception("rise exception"));
var_dump($gen->current());
$gen->send("send foo");

?>
--EXPECT--
string(15) "start generator"
string(9) "yield foo"
string(14) "rise exception"
string(9) "yield bar"
string(8) "send foo"
