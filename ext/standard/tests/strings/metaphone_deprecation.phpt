--TEST--
metaphone() deprecation
--FILE--
<?php

var_dump(metaphone("programming"));

?>
--EXPECTF--
Deprecated: Function metaphone() is deprecated since 8.6 in %s on line %d
string(7) "PRKRMNK"
