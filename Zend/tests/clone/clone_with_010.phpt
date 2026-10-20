--TEST--
Clone with native classes
--FILE--
<?php

try {
	var_dump(clone(new \Random\Engine\Secure(), [ 'with' => "something" ]));
} catch (Throwable $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
	var_dump(clone(new \Random\Engine\Xoshiro256StarStar(), [ 'with' => "something" ]));
} catch (Throwable $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class Random\Engine\Secure
Error: Cannot create dynamic property Random\Engine\Xoshiro256StarStar::$with
