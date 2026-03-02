--TEST--
Bug #51934 (strtotime plurals / incorrect time)
--FILE--
<?php
date_default_timezone_set('America/Los_Angeles');

echo date("Y-m-d H:i:s", strtotime("2010-05-27 19:18 4 Sunday ago")), "\n";
echo date("Y-m-d H:i:s", strtotime("2010-05-27 19:18 4 Sundays ago")), "\n";
?>
--EXPECTF--
2010-05-02 19:18:00
2010-05-02 19:18:00
