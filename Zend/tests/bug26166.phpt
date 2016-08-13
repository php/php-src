--TEST--
Bug #26166 (__toString() crash when no values returned)
--FILE--
<?php

class Foo
{
    function __toString()
    {
        return "Hello World!\n";
    }
}

class Bar
{
    private $obj;

    function __construct()
    {
        $this->obj = new Foo();
    }

    function __toString()
    {
        return $this->obj->__toString();
    }
}

$o = new Bar;
echo $o;

echo "===NONE===\n";

function my_error_handler($errno, $errstr, $errfile, $errline) {
	var_dump($errstr);
}

set_error_handler('my_error_handler');

class NoneTest
{
	function __toString() {
	}
}

$o = new NoneTest;
echo $o;

echo "===THROW===\n";

class ErrorTest
{
	function __toString() {
		throw new Exception("This is an error!");
	}
}

$o = new ErrorTest;
try {
	echo $o;
}
catch (Exception $e) {
	echo "Got the exception\n";
}

?>
===DONE===
--EXPECTF--
Hello World!
===NONE===
string(%d) "Method NoneTest::__toString() must return a string value"
===THROW===

Fatal error: Method ErrorTest::__toString() must not throw an exception, caught Exception: This is an error! in %sbug26166.php on line %d
