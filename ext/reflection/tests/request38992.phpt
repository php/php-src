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
try {
    $r->invoke('WTF?');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $r->invokeArgs('WTF?', array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
ReflectionMethod::invoke() expects argument #1 ($object) to be of type object, string given
ReflectionMethod::invokeArgs() expects argument #1 ($object) to be of type object, string given
