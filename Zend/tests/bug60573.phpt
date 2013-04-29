--TEST--
Bug #60573 (type hinting with "self" keyword causes weird errors)
--FILE--
<?php
class Foo1 {

public function setSelf(self $s) { }

}

class Bar1 extends Foo1 {

public function setSelf(parent $s) { }

}

class Foo2 {

public function setSelf(Foo2 $s) { }

}

class Bar2 extends Foo2 {

public function setSelf(parent $s) { }

}

class Base {
}

class Foo3 extends Base{

public function setSelf(parent $s) { }

}

class Bar3 extends Foo3 {

public function setSelf(Base $s) { }

}

class Foo4 {

public function setSelf(self $s) { }

}

class Bar4 extends Foo4 {

public function setSelf(self $s) { }

}

class Foo5 extends Base {

public function setSelf(parent $s) { }

}

class Bar5 extends Foo5 {

public function setSelf(parent $s) { }

}

abstract class Foo6 extends Base {

abstract public function setSelf(parent $s);

}

class Bar6 extends Foo6 {

public function setSelf(Foo6 $s) { }

}
--EXPECTF--
Strict Standards: Declaration of Bar4::setSelf() should be compatible with Foo4::setSelf(Foo4 $s) in %sbug60573.php on line %d

Strict Standards: Declaration of Bar5::setSelf() should be compatible with Foo5::setSelf(Base $s) in %sbug60573.php on line %d

Fatal error: Declaration of Bar6::setSelf() must be compatible with Foo6::setSelf(Base $s) in %sbug60573.php on line %d
