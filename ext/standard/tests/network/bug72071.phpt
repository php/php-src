--TEST--
Bug #72071 setcookie allows max-age to be negative
--INI--
date.timezone=UTC
--FILE--
<?php

$date = mktime(12, 25, 39, 4, 1, 2017);
setcookie("name", "value", $date);

?>
--EXPECT--
--EXPECTHEADERS--
Set-Cookie: name=value; expires=Sat, 01 Apr 2017 12:25:39 GMT; Max-Age=0
