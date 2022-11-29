--TEST--
SPL: SplFileObject::setMaxLineLen
--CREDITS--
H�vard Eide <nucleuz at gmail.com>
#Testfest php.no
--INI--
include_path=.
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
$s->setMaxLineLen( 3);
echo $s->getCurrentLine();
?>
--EXPECT--
<?p
