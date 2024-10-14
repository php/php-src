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

?>
--EXPECTF--
Fatal error: Declaration of Bar4::setSelf(Bar4 $s) must be compatible with Foo4::setSelf(Foo4 $s) in %s on line %d
