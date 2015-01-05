--TEST--
Bug #29150 (Roman number format for months)
--INI--
date.timezone=GMT
--FILE--
<?php
	echo gmdate("Y-m-d H:i:s", strtotime("20 VI. 2005"));
?>
--EXPECT--
2005-06-20 00:00:00
