--TEST--
Bug #48276 (date("Y") prints wrong year on Big Endian machines)
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump(date("Y", 1245623227));
?>
--EXPECT--
string(4) "2009"
