--TEST--
Ensure that DOMImplementation::getFeature triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$d = new DOMImplementation();
echo $d->getFeature();
?>
--EXPECTF--	
Warning: DOMImplementation::getFeature(): Not yet implemented in %s on line %d