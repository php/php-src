--TEST--
Ensure that DOMConfiguration::getParameter triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$dom_config = new DOMConfiguration();
$dom_config->getParameter('param');
?>
--EXPECTF--	
Warning: DOMConfiguration::getParameter(): Not yet implemented in %s on line %d