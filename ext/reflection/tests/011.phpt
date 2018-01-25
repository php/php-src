--TEST--
ReflectionExtension::getClasses()
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
$ext = new ReflectionExtension("reflection");
$classes = $ext->getClasses();
echo $classes["ReflectionException"]->getName();
?>
--EXPECT--
ReflectionException
