--TEST--
Generic syntax: variance + bound + default combined
--FILE--
<?php
class Bar {}
class Box<out T : Bar = Bar> {}
$p = (new ReflectionClass('Box'))->getGenericParameters()[0];
echo $p, "\n";
?>
--EXPECT--
out T : Bar = Bar
