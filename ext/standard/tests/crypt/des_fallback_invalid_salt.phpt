--TEST--
Test DES with invalid fallback
--FILE--
<?php

var_dump(crypt("test", "$#"));
var_dump(crypt("test", "$5zd$01"));

?>
--EXPECTF--
Deprecated: crypt(): Supplied salt is not valid for DES. Possible bug in provided salt format. in %s on line %d
string(13) "$#8MWASl5pGIk"

Deprecated: crypt(): Supplied salt is not valid for DES. Possible bug in provided salt format. in %s on line %d
string(13) "$54mkQyGCLvHs"