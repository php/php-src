--TEST--
Ensure that DOMUserDataHandler::handle triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$foo = new DOMUserDataHandler();
$foo->handle();
?>
--EXPECTF--	
Warning: DOMUserDataHandler::handle(): Not yet implemented in %s on line %d