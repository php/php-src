--TEST--
Bug #26694 (strtotime() request for "Sun, 21 Dec 2003 20:38:33 +0000 GMT")
--INI--
date.timezone=GMT
--FILE--
<?php
	echo gmdate("Y-m-d H:i:s", strtotime("Sun, 21 Dec 2003 20:38:33 +0000 GMT"));
?>
--EXPECT--
2003-12-21 20:38:33
