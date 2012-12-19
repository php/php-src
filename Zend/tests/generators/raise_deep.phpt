--TEST--
Raise exception in the generator
--FILE--
<?php
function gen() {
    var_dump("start generator");
    try {
		var_dump(yield gen2());
	} catch(Exception $e) {
		var_dump($e->getMessage());
		throw $e;
	}
    var_dump(yield "yield bar");
}

function gen2() {
	var_dump("start sub generator");
	
	var_dump(yield "yield foo");
}

$gen = gen();
var_dump($gen->current());
$gen->raise(new Exception("raise exception"));
var_dump($gen->current());
$gen->send("send foo");

?>
--EXPECT--
string(15) "start generator"
string(9) "yield foo"
string(14) "raise exception"
string(14) "exception dtor"
string(9) "yield bar"
string(8) "send foo"
