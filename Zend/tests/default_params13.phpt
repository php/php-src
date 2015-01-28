--TEST--
Default parameters - 13, disallowed defaults
--INI--
date.timezone=UTC
--FILE--
<?php
mktime(default, 0, 0, 7, 1, 2000);
echo "Done\n";
?>
--EXPECTF--
Warning: Default can not be used for parameter 1 in %s/default_params13.php on line 2
Done