--TEST--
Using disable_functions INI to remove exit
--INI--
disable_functions=exit
--FILE--
<?php

exit();

?>
--EXPECT--
Warning: Cannot disable function exit() in Unknown on line 0
