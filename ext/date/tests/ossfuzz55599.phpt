--TEST--
oss-fuzz 55599 (Null-dereference READ in restore_custom_datetime_properties)
--FILE--
<?php

try {
    unserialize('O:8:"DateTime":1:{i:1;d:2;');
} catch (Error $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
%s: unserialize(): Error at offset 26 of 26 bytes in %s on line %d
string(46) "Invalid serialization data for DateTime object"
