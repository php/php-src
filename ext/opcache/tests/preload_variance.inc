<?php

// Requires X, delay to runtime.
// TODO: It is not actually required, because we don't need X to check inheritance in this case.
class A extends Z {
    public function method(X $a) {}
}
class B extends Z {
    public function method($a) : X {}
}

// Works.
class C extends Z {
    public function method($a): self {}
    public function method2($a): C {}
    public function method3($a): object {}
    public function method4(D $a) {}
}
class D extends C {
    public function method($a): self {}
    public function method2($a): D {}
    public function method3($a): stdClass {}
    public function method4(C $a) {}
}

// Works.
interface I {}
interface J extends I {}
class E {
    public I|J $prop;
    public function method($a): I {}
}
class F extends E {
    public J|I $prop;
    public function method($a): J {}
}

// Requires K & L, delay to runtime.
class G {
    public function method($a): K {}
}
class H extends G {
    public function method($a): L {}
}

// Early-binding preventer.
class Z {
    public function method(X $a) {}
}
