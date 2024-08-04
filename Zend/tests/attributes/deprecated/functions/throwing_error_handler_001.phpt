--TEST--
#[\Deprecated]: Throwing error handler.
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

#[\Deprecated("convert to exception")]
function test() {
	echo "Not executed", PHP_EOL;
}

try {
	test();
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

eval(<<<'CODE'
	#[\Deprecated("convert to exception")]
	function test2() {
		echo "Not executed", PHP_EOL;
	}
CODE);

try {
	test2();
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

class Clazz {
	#[\Deprecated("convert to exception")]
	function test() {
		echo "Not executed", PHP_EOL;
	}
}

try {
	$cls = new Clazz();
	$cls->test();
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

$closure = #[\Deprecated("convert to exception")] function () {
	echo "Not executed", PHP_EOL;
};

try {
	$closure();
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

class Constructor {
	#[\Deprecated("convert to exception")]
	public function __construct() {
		echo "Not executed", PHP_EOL;
	}
}

try {
	new Constructor();
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

class Destructor {
	#[\Deprecated("convert to exception")]
	public function __destruct() {
		echo "Not executed", PHP_EOL;
	}
}

try {
	new Destructor();
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Caught: Function test() is deprecated, convert to exception
Caught: Function test2() is deprecated, convert to exception
Caught: Method Clazz::test() is deprecated, convert to exception
Caught: Function {closure:%s:%d}() is deprecated, convert to exception
Caught: Method Constructor::__construct() is deprecated, convert to exception
Caught: Method Destructor::__destruct() is deprecated, convert to exception
