--TEST--
SPL: SplFileInfo::getBasename() basic test
--FILE--
<?php
// without $suffix
echo (new \SplFileInfo('/path/to/a.txt'))->getBasename() . PHP_EOL;
echo (new \SplFileInfo('path/to/b'))->getBasename() . PHP_EOL;
echo (new \SplFileInfo('c.txt'))->getBasename() . PHP_EOL;
echo (new \SplFileInfo('d'))->getBasename() . PHP_EOL;
echo (new \SplFileInfo('~/path/to//e'))->getBasename() . PHP_EOL . PHP_EOL;

// with $suffix
echo (new \SplFileInfo('path/to/a.txt'))->getBasename('.txt') . PHP_EOL;
echo (new \SplFileInfo('path/to/bbb.txt'))->getBasename('b.txt') . PHP_EOL;
echo (new \SplFileInfo('path/to/ccc.txt'))->getBasename('to/ccc.txt') . PHP_EOL;
echo (new \SplFileInfo('d.txt'))->getBasename('txt') . PHP_EOL;
echo (new \SplFileInfo('e.txt'))->getBasename('e.txt') . PHP_EOL;
echo (new \SplFileInfo('f'))->getBasename('.txt');
?>
--EXPECT--
a.txt
b
c.txt
d
e

a
bb
ccc.txt
d.
e.txt
f
