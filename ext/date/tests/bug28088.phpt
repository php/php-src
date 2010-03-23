--TEST--
Bug #28088 (strtotime() cannot convert 00 hours")
--FILE--
<?php
putenv("TZ=GMT");

echo "The following line rightly shows the correct date time:\n";
echo gmdate("m/d/y Hi", strtotime("04/04/04 2345")), "\n";

echo "But the following line fails to show the correct date time:\n";
echo gmdate("m/d/y Hi", strtotime("04/04/04 0045"))."\r\n";
?>
--EXPECT--
The following line rightly shows the correct date time:
04/04/04 2345
But the following line fails to show the correct date time:
04/04/04 0045
