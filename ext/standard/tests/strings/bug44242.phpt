--TEST--
Bug #44242 (metaphone('CMXFXM') crashes PHP)
--FILE--
<?php

echo metaphone('CMXFXZ'), "\n";
echo metaphone('CMXFXV'), "\n";
echo metaphone('CMXFXZXZ'), "\n";

?>
--EXPECT--
KMKSFKSS
KMKSFKSF
KMKSFKSSKSS
