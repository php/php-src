--TEST--
Bug #37747 (strtotime segfaults when given "nextyear")
--FILE--
<?php
    date_default_timezone_set("Europe/Oslo");
    var_dump(strtotime("nextyear"));
    echo "ALIVE\n";
?>
--EXPECT--
bool(false)
ALIVE
