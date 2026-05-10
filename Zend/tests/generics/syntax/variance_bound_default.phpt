--TEST--
Generic syntax: variance + bound + default combined
--FILE--
<?php
class Bar {}
class Box<+T : Bar = Bar> {}
$p = (new ReflectionClass('Box'))->getGenericParameters()[0];
echo $p, "\n";
?>
--EXPECT--
+T : Bar = Bar
