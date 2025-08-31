--TEST--
Bug #70451 IntlChar::charFromName() not consistent with C library or HHVM
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(IntlChar::charFromName("RECYCLING SYMBOL FOR TYPE-1 PLASTICS"));
var_dump(IntlChar::charFromName("sdfasdfasdfasdf"));
?>
--EXPECT--
int(9843)
NULL
