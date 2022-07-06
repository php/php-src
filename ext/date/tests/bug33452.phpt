--TEST--
Bug #33452 (Support for year accompanying ISO week nr)
--FILE--
<?php
date_default_timezone_set("GMT");
echo date('Y-W', strtotime('2005-1-1')), "\n";
echo date('o-W', strtotime('2005-1-1')), "\n";
?>
--EXPECT--
2005-53
2004-53
