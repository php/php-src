--TEST--
Bug #44242 (metaphone('CMXFXM') crashes PHP)
--FILE--
<?php

echo metaphone('CMXFXZ'), "\n";
echo metaphone('CMXFXV'), "\n";
echo metaphone('CMXFXZXZ'), "\n";

?>
--EXPECTF--
Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
KMKSFKSS

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
KMKSFKSF

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
KMKSFKSSKSS
