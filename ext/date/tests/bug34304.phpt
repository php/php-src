--TEST--
Bug #34304 (date('w') returns wrong number for sunday, 'N' modifier is missing)
--FILE--
<?php
date_default_timezone_set("UTC");
echo date('o\-\WW\-N', strtotime('2 January 2005')), "\n";
echo date('o\-\WW\-N', strtotime('9 January 2005')), "\n";
?>
--EXPECT--
2004-W53-7
2005-W01-7
