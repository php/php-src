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

echo "===THROW===\n";

class Error 
{
	function __toString() {
		throw new Exception("This is an error!");
	}
}

$o = new Error;
try {
	echo $o;
}
catch (Exception $e) {
	echo "Got the exception\n";
}

echo "===NONE===\n";

class None
{
	function __toString() {
	}
}

$o = new None;
echo $o;

?>
===DONE===
--EXPECTF--
Hello World!
===THROW===
Got the exception
===NONE===

Fatal error: Method None::__toString() must return a string value in %sbug26166.php on line %d
