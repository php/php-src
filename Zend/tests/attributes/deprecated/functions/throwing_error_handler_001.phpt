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
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

eval(<<<'CODE'
	#[\Deprecated("convert to exception")]
	function test2() {
		echo "Not executed", PHP_EOL;
	}
CODE);

try {
	test2();
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
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
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

$closure = #[\Deprecated("convert to exception")] function () {
	echo "Not executed", PHP_EOL;
};

try {
	$closure();
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

class Constructor {
	#[\Deprecated("convert to exception")]
	public function __construct() {
		echo "Not executed", PHP_EOL;
	}
}

try {
	new Constructor();
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

class Destructor {
	#[\Deprecated("convert to exception")]
	public function __destruct() {
		echo "Not executed", PHP_EOL;
	}
}

try {
	new Destructor();
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
ErrorException: Function test() is deprecated, convert to exception
ErrorException: Function test2() is deprecated, convert to exception
ErrorException: Method Clazz::test() is deprecated, convert to exception
ErrorException: Function {closure:%s:%d}() is deprecated, convert to exception
ErrorException: Method Constructor::__construct() is deprecated, convert to exception
ErrorException: Method Destructor::__destruct() is deprecated, convert to exception
