--TEST--
Bug #74080 Provide an RFC7231 date time format
--FILE--
<?php

$date = mktime(17, 52, 13, 4, 30, 2016);
var_dump(date(\DateTime::RFC7231, $date));

?>
--EXPECTF--
string(29) "Sat, 30 Apr 2016 17:52:13 GMT"
