--TEST--
iptcembed() unable to open file
--CREDITS--
Mark Niebergall <mbniebergall@gmail.com>
PHP TestFest 2017 - UPHPU
--FILE--
<?php
var_dump(iptcembed(-1, __DIR__ . '/iptcembed/DOES_NOT_EXIST.ERR', -1));
?>
--EXPECTF--
Warning: iptcembed(): Unable to open %s in %s on line %d
bool(false)
