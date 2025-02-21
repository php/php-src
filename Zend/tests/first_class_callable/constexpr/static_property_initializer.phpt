--TEST--
FCC in static property initializer
--FILE--
<?php

class C {
    public static Closure $d = strrev(...);
}

var_dump(C::$d);
var_dump((C::$d)("abc"));


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
