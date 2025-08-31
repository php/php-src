--TEST--
SPL: SplFileInfo::getFilename() basic test
--FILE--
<?php

echo (new \SplFileInfo('/path/to/a.txt'))->getFilename() . PHP_EOL;
echo (new \SplFileInfo('path/to/b'))->getFilename() . PHP_EOL;
echo (new \SplFileInfo('c.txt'))->getFilename() . PHP_EOL;
echo (new \SplFileInfo('d'))->getFilename() . PHP_EOL;
echo (new \SplFileInfo('~/path/to//e'))->getFilename() . PHP_EOL . PHP_EOL;

?>
--EXPECT--
a.txt
b
c.txt
d
e
