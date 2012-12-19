--TEST--
Raise exception in the generator
--FILE--
<?php
class ExceptionEx extends Exception {
	public function __destruct() {
		var_dump("exception dtor");
	}
}
function gen() {
    var_dump("start generator");
    try {
        var_dump(yield "yield foo");
        var_dump("throws failed");
    } catch(Exception $e) {
        var_dump($e->getMessage());
        unset($e);
    }
    var_dump(yield "yield bar");
}

$gen = gen();
var_dump($gen->current());
$gen->raise(new ExceptionEx("raise exception"));
var_dump($gen->current());
$gen->send("send foo");

?>
--EXPECT--
string(15) "start generator"
string(9) "yield foo"
string(15) "raise exception"
string(14) "exception dtor"
string(9) "yield bar"
string(8) "send foo"
