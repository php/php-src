--TEST--
Bug #60573 (type hinting with "self" keyword causes weird errors) -- variation 2
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

class Foo5 extends Base {

public function setSelf(parent $s) { }

}

class Bar5 extends Foo5 {

public function setSelf(parent $s) { }

}

?>
--EXPECTF--
Fatal error: Declaration of Bar5::setSelf(Foo5 $s) must be compatible with Foo5::setSelf(Base $s) in %sbug60573_2.php on line %d
