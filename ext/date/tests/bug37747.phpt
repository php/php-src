--TEST--
Bug #37747 (strtotime segfaults when given "nextyear")
--FILE--
<?php
    var_dump(strtotime("nextyear"));
	echo "ALIVE\n";
?>
--EXPECT--
bool(false)
ALIVE
