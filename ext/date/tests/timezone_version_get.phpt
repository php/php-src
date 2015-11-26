--TEST--
Test the basics to function timezone_version_get().
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--INI--
date.timezone = UTC;
date_default_timezone_set("America/Sao_Paulo"); 
--FILE--
<?php
var_dump(timezone_version_get());
?>
--EXPECTREGEX--
string\([6-7]\) \"20[0-9][0-9]\.[1-9][0-9]?\"