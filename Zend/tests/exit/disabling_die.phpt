--TEST--
Using disable_functions INI to remove die
--INI--
disable_functions=die
--FILE--
<?php

die();

?>
--EXPECT--
