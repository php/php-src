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

$o = new NoneTest;
try {
    echo $o;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
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
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}

?>
===DONE===
--EXPECT--
Hello World!
===NONE===
Method NoneTest::__toString() must return a string value
===THROW===
This is an error!
===DONE===
