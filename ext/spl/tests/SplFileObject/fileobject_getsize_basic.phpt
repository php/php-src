--TEST--
SPL: SplFileObject::getSize
--CREDITS--
H�vard Eide <nucleuz at gmail.com>
#Testfest php.no
--INI--
include_path=.
--FILE--
<?php
$file = __DIR__ ."/data.txt";
file_put_contents($file, "foobar");

$s = new SplFileObject( $file );
echo $s->getSize();
?>
--CLEAN--
<?php
$file = __DIR__ ."/data.txt";
unlink($file);
?>
--EXPECT--
6
