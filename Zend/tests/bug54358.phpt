--TEST--
Bug #54358 (Closure, use and reference)
--FILE--
<?php
class asserter {
	public function call($function) {
	}
}

$asserter = new asserter();

$closure = function() use ($asserter, &$function) {
        $asserter->call($function = 'md5');
};

$closure();

var_dump($function);

$closure = function() use ($asserter, $function) {
        $asserter->call($function);
};

$closure();

var_dump($function);

$closure = function() use ($asserter, $function) {
        $asserter->call($function);
};

$closure();

var_dump($function);
?>
--EXPECT--
string(3) "md5"
string(3) "md5"
string(3) "md5"
