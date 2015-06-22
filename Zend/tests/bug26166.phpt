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

class NoneTest
{
	function __toString() {
	}
}

try {
    $o = new NoneTest;
    echo $o;
} catch (Error $e) {
    echo $e, "\n";
}

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
Error: Method NoneTest::__toString() must return a string value in %s:%d
Stack trace:
#0 {main}
===THROW===
Got the exception
===DONE===
