--TEST--
Check if static classes are supported
--FILE--
<?php
static class A
{
    public static $c = "foo\n";
    public static function display($name)
    {
        echo "Hello $name\n";
    }
}
echo A::$c;
A::display('Foo');
$a = new A;
?>
--EXPECTF--
foo
Hello Foo

Fatal error: Cannot instantiate static class %s in %s on line %d
