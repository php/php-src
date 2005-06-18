--TEST--
date.timezone setting [1]
--INI--
date.timezone=GMT
--FILE--
<?php
	echo strtotime("2005-06-18 22:15:44");
?>
--EXPECT--
1119132944
