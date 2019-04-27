--TEST--
errmsg: disabled function
--INI--
disable_functions=phpinfo
--FILE--
<?php

phpinfo();

echo "Done\n";
?>
--EXPECTF--
Warning: phpinfo() has been disabled for security reasons in %s on line %d
Done
