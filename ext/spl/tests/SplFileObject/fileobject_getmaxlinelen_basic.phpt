--TEST--
SPL: SplFileObject::getMaxLineLen()
--CREDITS--
H�vard Eide <nucleuz at gmail.com>
#Testfest php.no
--INI--
include_path=.
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
$s->setMaxLineLen( 7 );
echo $s->getMaxLineLen();
?>
--EXPECT--
7
