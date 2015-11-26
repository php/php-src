--TEST--
SPL: SplFileObject::isFile/isDir/isLink
--CREDITS--
H�vard Eide <nucleuz at gmail.com>
#Testfest php.no
--FILE--
<?php
$s = new SplFileObject(__FILE__);
var_dump($s->isFile());
var_dump($s->isDir());
var_dump($s->isLink());
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
