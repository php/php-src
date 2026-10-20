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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Hello World!
===NONE===
TypeError: NoneTest::__toString(): Return value must be of type string, none returned
===THROW===
Exception: This is an error!
