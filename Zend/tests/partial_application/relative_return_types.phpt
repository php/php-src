--TEST--
PFA supports relative return types
--FILE--
<?php

if (time() > 0) {
    trait T {
        public function getSelf(object $o): self {
            return $o;
        }
        public function getStatic(object $o): static {
            return $o;
        }
    }
}

class C {
    use T;
}

class D extends C {
}

$c = new C;

$self = $c->getSelf(?);

echo (string) new ReflectionFunction($self), "\n";

var_dump($self($c));
var_dump($self(new D));
try {
    $self(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$static = $c->getStatic(?);

echo (string) new ReflectionFunction($static), "\n";

var_dump($static($c));
var_dump($static(new D));
try {
    $static(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$d = new D;

$self = $d->getSelf(?);

echo (string) new ReflectionFunction($self), "\n";

var_dump($self($d));
var_dump($self(new D));
try {
    $self(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$static = $d->getStatic(?);

echo (string) new ReflectionFunction($static), "\n";

var_dump($static($d));
var_dump($static(new D));
try {
    $static(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s.php 23 - 23

  - Parameters [1] {
    Parameter #0 [ <required> object $o ]
  }
  - Return [ self ]
}

object(C)#%d (0) {
}
object(D)#%d (0) {
}
C::getSelf(): Return value must be of type C, stdClass returned
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s.php 35 - 35

  - Parameters [1] {
    Parameter #0 [ <required> object $o ]
  }
  - Return [ static ]
}

object(C)#%d (0) {
}
object(D)#%d (0) {
}
C::getStatic(): Return value must be of type C, stdClass returned
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s.php 49 - 49

  - Parameters [1] {
    Parameter #0 [ <required> object $o ]
  }
  - Return [ self ]
}

object(D)#%d (0) {
}
object(D)#%d (0) {
}
C::getSelf(): Return value must be of type C, stdClass returned
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s.php 61 - 61

  - Parameters [1] {
    Parameter #0 [ <required> object $o ]
  }
  - Return [ static ]
}

object(D)#%d (0) {
}
object(D)#%d (0) {
}
C::getStatic(): Return value must be of type D, stdClass returned
