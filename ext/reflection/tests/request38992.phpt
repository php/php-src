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
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $r->invokeArgs('WTF?', array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: ReflectionMethod::invoke(): Argument #1 ($object) must be of type ?object, string given
TypeError: ReflectionMethod::invokeArgs(): Argument #1 ($object) must be of type ?object, string given
