--TEST--
oss-fuzz 55589 (Null-dereference in zim_DateInterval___unserialize)
--FILE--
<?php

try {
    unserialize('O:12:"DateInterval":2:{i:2;r:1;i:0;R:2;');
} catch (Error $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
%s: unserialize(): Error at offset 39 of 39 bytes in %s on line %d
