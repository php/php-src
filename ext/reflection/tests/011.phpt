--TEST--
ReflectionExtension::getClasses()
--FILE--
<?php
$ext = new ReflectionExtension("reflection");
$classes = $ext->getClasses();
echo $classes["ReflectionException"]->getName();
?>
--EXPECT--
ReflectionException
