--TEST--
Bug #33456 (strtotime defaults to now even on non time string)
--FILE--
<?php
date_default_timezone_set("GMT");
var_dump(strtotime("monkey"));
print date("Y-m-d", strtotime("monkey")) ."\n";
print date("Y-m-d", false) ."\n";
?>
--EXPECT--
bool(false)
1970-01-01
1970-01-01
