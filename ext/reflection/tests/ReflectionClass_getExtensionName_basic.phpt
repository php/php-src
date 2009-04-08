--TEST--
ReflectionClass::getExtensionName() method - basic test for getExtensionName() method
--SKIPIF--
<?php extension_loaded('reflection') && extension_loaded('dom') or die('skip - reflection or dom extension not loaded'); ?>
--CREDITS--
Rein Velt <rein@velt.org>
#testFest Roosendaal 2008-05-10
--FILE--
<?php
 	$rc=new reflectionClass('domDocument');
 	var_dump( $rc->getExtensionName()) ;
?>
--EXPECT--
string(3) "dom"
