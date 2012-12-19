--TEST--
Raise exception in the generator
--FILE--
<?php
function gen() {
    var_dump("start generator");
	var_dump(yield "yield foo");
	try {
		
		var_dump("throws failed");
	} catch(Exception $e) {
	var_dump("catch");
		throw $e;
	}
    
    var_dump(yield "yield bar");
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
