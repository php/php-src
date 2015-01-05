--TEST--
SPL: SplFileObject::getBasename 
--CREDITS--
H�vard Eide <nucleuz at gmail.com>
#Testfest php.no
--FILE--
<?php
$file = __FILE__;
$s = new SplFileObject( __FILE__ );
echo $s->getBasename();
?>
--EXPECT--
fileobject_getbasename_basic.php
