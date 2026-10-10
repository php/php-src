--TEST--
ReflectionExtension::getClassNames() lists the Io\Ring classes of a build with ior
--SKIPIF--
<?php
if (!class_exists('Io\\Ring\\Engine')) die('skip built without ior');
?>
--FILE--
<?php
$standard = new ReflectionExtension('standard');
$classNames = array_filter($standard->getClassNames(), fn ($c) => str_starts_with($c, 'Io\\Ring\\'));
sort($classNames);
foreach ($classNames as $className) {
    echo $className, PHP_EOL;
}
?>
--EXPECT--
Io\Ring\Backend
Io\Ring\Engine
Io\Ring\FailedRingOperationException
Io\Ring\RingException
