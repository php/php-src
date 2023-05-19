--TEST--
Lazy objects: destructor of initialized objets is called
--FILE--
<?php

class C {
    public int $a = 1;

    public function __destruct() {
        var_dump(__METHOD__, $this);
    }
}

function ghost() {
    print "# Ghost:\n";

    print "In makeLazy\n";
    $obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
    (new ReflectionClass($obj))->resetAsLazyGhost($obj, function () {
        var_dump("initializer");
    });
    print "After makeLazy\n";

    var_dump($obj->a);
}

function virtual() {
    print "# Virtual:\n";

    print "In makeLazy\n";
    $obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
    (new ReflectionClass($obj))->resetAsLazyProxy($obj, function () {
        var_dump("initializer");
        return new C();
    });
    print "After makeLazy\n";

    var_dump($obj->a);
}

ghost();
virtual();

--EXPECTF--
# Ghost:
In makeLazy
string(13) "C::__destruct"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
After makeLazy
string(11) "initializer"
int(1)
string(13) "C::__destruct"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
# Virtual:
In makeLazy
string(13) "C::__destruct"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
After makeLazy
string(11) "initializer"
int(1)
string(13) "C::__destruct"
object(C)#%d (1) {
  ["a"]=>
  int(1)
}
