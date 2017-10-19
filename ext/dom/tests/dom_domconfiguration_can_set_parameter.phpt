--TEST--
Ensure that DOMConfiguration::canSetParameter triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$dom_config = new DOMConfiguration();
$dom_config->canSetParameter('param');
?>
--EXPECTF--	
Warning: DOMConfiguration::canSetParameter(): Not yet implemented in %s on line %d