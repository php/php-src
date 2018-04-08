--TEST--
Request #38992 (invoke() and invokeArgs() static method calls should match)
--FILE--
<?php
class MyClass
{
    public static function doSomething()
    {
        echo "Did it!\n";
    }
}

$r = new ReflectionMethod('MyClass', 'doSomething');
$r->invoke('WTF?');
$r->invokeArgs('WTF?', array());
?>
===DONE===
--EXPECTF--
Warning: ReflectionMethod::invoke() expects parameter 1 to be object, string given in %s%erequest38992.php on line %d

Warning: ReflectionMethod::invokeArgs() expects parameter 1 to be object, string given in %s%erequest38992.php on line %d
===DONE===
