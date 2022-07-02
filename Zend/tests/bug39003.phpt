--TEST--
Bug #39003 (autoloader is called for type hinting)
--FILE--
<?php

class ClassName
{
    public $var = 'bla';
}

function test (OtherClassName $object) { }

spl_autoload_register(function ($class) {
    var_dump("__autload($class)");
});

$obj = new ClassName;
test($obj);

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: test(): Argument #1 ($object) must be of type OtherClassName, ClassName given, called in %s:%d
Stack trace:
#0 %s(%d): test(Object(ClassName))
#1 {main}
  thrown in %s on line %d
