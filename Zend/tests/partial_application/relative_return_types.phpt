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

echo "# C::getSelf():\n";

$self = $c->getSelf(?);

echo (string) new ReflectionFunction($self), "\n";

var_dump($self($c));
var_dump($self(new D));
try {
    $self(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "# C::getStatic():\n";

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

echo "# D::getSelf():\n";

$self = $d->getSelf(?);

echo (string) new ReflectionFunction($self), "\n";

var_dump($self($d));
var_dump($self(new C));
try {
    $self(new stdClass);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

echo "# D::getStatic():\n";

$static = $d->getStatic(?);

echo (string) new ReflectionFunction($static), "\n";

var_dump($static($d));
try {
    var_dump($static(new C));
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
try {
    $static(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
# C::getSelf():
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 25 - 25

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
# C::getStatic():
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 39 - 39

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
# D::getSelf():
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 55 - 55

  - Parameters [1] {
    Parameter #0 [ <required> object $o ]
  }
  - Return [ self ]
}

object(D)#%d (0) {
}
object(C)#%d (0) {
}
TypeError: C::getSelf(): Return value must be of type C, stdClass returned
# D::getStatic():
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 69 - 69

  - Parameters [1] {
    Parameter #0 [ <required> object $o ]
  }
  - Return [ static ]
}

object(D)#%d (0) {
}
TypeError: C::getStatic(): Return value must be of type D, C returned
C::getStatic(): Return value must be of type D, stdClass returned
