--TEST--
GH-12532 (PharData created from zip has incorrect timestamp)
--EXTENSIONS--
phar
--FILE--
<?php

date_default_timezone_set('UTC');
$phar = new PharData(__DIR__ . '/gh12532.zip');
echo $phar->getMTime(), "\n";
echo $phar->getFileInfo()->getMTime(), "\n";
echo date('Y-m-d H:i:s', $phar->getMTime()), "\n";
echo date('Y-m-d H:i:s', $phar->getCTime()), "\n";
echo date('Y-m-d H:i:s', $phar->getATime()), "\n";

?>
--EXPECT--
1680284661
1680284661
2023-03-31 17:44:21
2023-03-31 17:44:21
2023-03-31 17:44:21
