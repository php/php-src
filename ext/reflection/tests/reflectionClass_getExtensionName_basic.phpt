--TEST--
ReflectionClass::getExtensionName() method - basic test for getExtensionName() method
--SKIPIF--
<?php extension_loaded('dom') or die('skip - dom extension not loaded'); ?>
--CREDITS--
Rein Velt <rein@velt.org>
#testFest Roosendaal 2008-05-10
--FILE--
<?php
 	$rc=new reflectionClass('domDocument');
 	var_dump( $rc->getExtensionName()) ;
?>
--EXPECT--
unicode(3) "dom"
