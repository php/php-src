--TEST--
Lazy Objects: ReflectionProperty::isUninitializedLazy()
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public static $staticProp;
    public int $typed;
    public $untyped;
    public $virtual {
        get {}
    }
}

function testProps(ReflectionClass $reflector, object $obj) {
    foreach (['staticProp', 'typed', 'untyped', 'virtual', 'dynamic'] as $name) {
        if ('dynamic' === $name) {
            $tmp = new C();
            $tmp->dynamic = 1;
            $pr = new ReflectionProperty($tmp, $name);
        } else {
            $pr = $reflector->getProperty($name);
        }
        printf("%s: %d\n", $name, $pr->isUninitializedLazy($obj));
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost\n";

$obj = $reflector->newLazyGhost(function () { });

testProps($reflector, $obj);

$pr = $reflector->getProperty('typed');
$pr->skipLazyInitialization($obj);
printf("typed (skipped): %d\n", $pr->isUninitializedLazy($obj));

print "# Initialized Ghost\n";

$reflector->initializeLazyObject($obj);

testProps($reflector, $obj);

print "# Proxy\n";

$obj = $reflector->newLazyProxy(function () {
    return new C();
});

testProps($reflector, $obj);

$pr = $reflector->getProperty('typed');
$pr->skipLazyInitialization($obj);
printf("typed (skipped prop): %d\n", $pr->isUninitializedLazy($obj));

print "# Initialized Proxy\n";

$reflector->initializeLazyObject($obj);

testProps($reflector, $obj);

print "# Internal\n";

$obj = (new DateTime())->diff(new DateTime());
$reflector = new ReflectionClass(DateInterval::class);
$pr = new ReflectionProperty($obj, 'y');
printf("y: %d\n", $pr->isUninitializedLazy($obj));

?>
--EXPECT--
# Ghost
staticProp: 0
typed: 1
untyped: 1
virtual: 0
dynamic: 0
typed (skipped): 0
# Initialized Ghost
staticProp: 0
typed: 0
untyped: 0
virtual: 0
dynamic: 0
# Proxy
staticProp: 0
typed: 1
untyped: 1
virtual: 0
dynamic: 0
typed (skipped prop): 0
# Initialized Proxy
staticProp: 0
typed: 0
untyped: 0
virtual: 0
dynamic: 0
# Internal
y: 0
