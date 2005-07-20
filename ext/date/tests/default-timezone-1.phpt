--TEST--
date.timezone setting [1]
--INI--
date.timezone=GMT
--FILE--
<?php
	putenv('TZ='); // clean TZ so that it doesn't bypass the ini option
	echo strtotime("2005-06-18 22:15:44");
?>
--EXPECT--
1119132944
