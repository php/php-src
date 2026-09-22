--TEST--
Using disable_functions INI to remove die
--INI--
disable_functions=die
--FILE--
<?php

die();

?>
--EXPECT--
Warning: Cannot disable function die() in Unknown on line 0
