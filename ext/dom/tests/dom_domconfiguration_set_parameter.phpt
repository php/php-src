--TEST--
Ensure that DOMConfiguration::setParameter triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$dom_config = new DOMConfiguration();
$dom_config->setParameter('param');
?>
--EXPECTF--	
Warning: DOMConfiguration::setParameter(): Not yet implemented in %s on line %d