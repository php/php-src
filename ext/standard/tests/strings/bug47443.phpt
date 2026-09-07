--TEST--
Bug #47443 (metaphone('scratch') returns wrong result)
--FILE--
<?php

var_dump(metaphone("scratch"));
var_dump(metaphone("scrath"));
var_dump(metaphone("scratc"));

?>
--EXPECTF--
Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(4) "SKRX"

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(4) "SKR0"

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(5) "SKRTK"
