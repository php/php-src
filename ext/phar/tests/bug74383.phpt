--TEST--
Phar: bug #74383: Wrong reflection on Phar::running
--EXTENSIONS--
phar
--FILE--
<?php
$rc = new ReflectionClass(Phar::class);
$rm = $rc->getMethod("running");
echo $rm->getNumberOfParameters();
echo PHP_EOL;
echo $rm->getNumberOfRequiredParameters();
echo PHP_EOL;
echo (int) $rm->getParameters()[0]->isOptional();

?>
--EXPECT--
1
0
1
