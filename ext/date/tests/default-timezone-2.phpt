--TEST--
date.timezone setting [2]
--INI--
date.timezone=Europe/Oslo
--FILE--
<?php
	echo strtotime("2005-06-18 22:15:44");
?>
--EXPECT--
1119125744
