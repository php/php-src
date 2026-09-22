--TEST--
FCC in property initializer
--FILE--
<?php

class C {
    public Closure $d = strrev(...);
}

$c = new C();
var_dump($c->d);
var_dump(($c->d)("abc"));


?>
--EXPECTF--
object(Closure)#%d (2) {
  ["function"]=>
  string(6) "strrev"
  ["parameter"]=>
  array(1) {
    ["$string"]=>
    string(10) "<required>"
  }
}
string(3) "cba"
