--TEST--
Ensure that DOMErrorHandler::handleError triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$e = new DOMErrorHandler();
$e->handleError();
?>
--EXPECTF--	
Warning: DOMErrorHandler::handleError(): Not yet implemented in %s on line %d