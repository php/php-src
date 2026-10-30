--TEST--
Interface delegation: abstract method is not replaced by generated delegation method
--FILE--

<?php

interface A {
    public function foo(): string;
}

class AImpl implements A {
    public function foo(): string {
        return 'delegate';
    }
}

abstract class C implements A by $a {
    public function __construct(protected A $a) {}

    abstract public function foo(): string;
}

class D extends C {}

echo (new D(new AImpl))->foo(), "\n";
?>

--EXPECTF--
Fatal error: Class D contains 1 abstract method and must therefore be declared abstract or implement the remaining method (C::foo) in %s on line %d
