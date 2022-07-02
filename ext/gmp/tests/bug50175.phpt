--TEST--
Bug #50175: gmp_init() results 0 on given base and number starting with 0x or 0b
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_init('0bcd', 16));
var_dump(gmp_init('0xyz', 36));

?>
--EXPECTF--
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "3021"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(5) "44027"
}
