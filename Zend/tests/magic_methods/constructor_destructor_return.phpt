--TEST--
Returning values from constructors and destructors is deprecated
--FILE--
<?php

class A {
    public function __construct() { return ''; }
    public function __destruct() { return ''; }
}

class B {
    public function __construct() { return $this->voidMethod(); }
    public function __destruct() { return $this->voidMethod(); }

    public function voidMethod(): void { }
}

class Gen {
    public function __construct() { yield ''; }
    public function __destruct() { yield ''; }
}

?>
--EXPECTF--
Deprecated: Returning a value from a constructor is deprecated in %s on line %d

Deprecated: Returning a value from a destructor is deprecated in %s on line %d

Deprecated: Returning a value from a constructor is deprecated in %s on line %d

Deprecated: Returning a value from a destructor is deprecated in %s on line %d

Deprecated: Making a constructor a Generator is deprecated in %s on line %d

Deprecated: Making a destructor a Generator is deprecated in %s on line %d
