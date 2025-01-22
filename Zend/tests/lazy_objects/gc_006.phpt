--TEST--
Lazy objects: GC 006
--FILE--
<?php

class Foo {
    public $foo;
}

class Initializer {
    public function __invoke($obj) {
        $obj->foo = $this;
        var_dump(__METHOD__);
    }
    public function __destruct() {
        var_dump(__METHOD__);
    }
}

$reflector = new ReflectionClass(Foo::class);
$foo = $reflector->newLazyGhost(new Initializer());

print "Dump\n";

var_dump($foo->foo);

print "Done\n";

?>
--EXPECTF--
Dump
string(21) "Initializer::__invoke"
object(Initializer)#%d (0) {
}
Done
string(23) "Initializer::__destruct"
