--TEST--
GH-14969: Crash on coercion with throwing __toString()
--FILE--
<?php

class C {
    public function __toString() {
        global $c;
        $c = [];
        throw new Exception(__METHOD__);
    }
}

class D {
    public string $prop;
}

$c = new C();
$d = new D();
try {
    $d->prop = $c;
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
var_dump($d);

$c = new C();
$d->prop = 'foo';
try {
    $d->prop = $c;
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
var_dump($d);

?>
--EXPECTF--
C::__toString
object(D)#%d (0) {
  ["prop"]=>
  uninitialized(string)
}
C::__toString
object(D)#2 (1) {
  ["prop"]=>
  string(3) "foo"
}
