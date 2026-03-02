--TEST--
Bug #33456 (strtotime defaults to now even on non time string)
--FILE--
<?php
date_default_timezone_set("GMT");
var_dump(strtotime("monkey"));
?>
--EXPECT--
bool(false)
