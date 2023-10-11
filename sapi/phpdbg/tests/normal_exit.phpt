--TEST--
A script with die() must end "normally"
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Script ended normally]
prompt> 
--FILE--
<?php

(function($argv) {
	die();
})($argv);
?>
