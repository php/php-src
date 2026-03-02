--TEST--
Lazy objects: destructor of initialized objects is called
--FILE--
<?php

class C {
    public int $a = 1;

    public function __destruct() {
        var_dump(__METHOD__, $this);
    }
}

function ghost() {
    $reflector = new ReflectionClass(C::class);

    print "# Ghost:\n";

    print "In makeLazy\n";
    $obj = $reflector->newLazyGhost(function () {
        var_dump("initializer");
    });
    print "After makeLazy\n";

    var_dump($obj->a);
}

function proxy() {
    $reflector = new ReflectionClass(C::class);

    print "# Proxy:\n";

    print "In makeLazy\n";
    $obj = $reflector->newLazyProxy(function () {
        var_dump("initializer");
        return new C();
    });
    print "After makeLazy\n";

    var_dump($obj->a);
}

ghost();
proxy();

--EXPECTF--
# Ghost:
In makeLazy
After makeLazy
string(11) "initializer"
int(1)
string(13) "C::__destruct"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
# Proxy:
In makeLazy
After makeLazy
string(11) "initializer"
int(1)
string(13) "C::__destruct"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
