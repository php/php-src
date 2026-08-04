--TEST--
class_alias() with wrong-case class name fails with wrong case
--FILE--
<?php
class MyClass {}
class_alias("myclass", "Alias1");
class_alias("MYCLASS", "Alias2");
echo "done\n";
?>
--EXPECTF--
Warning: Class "myclass" not found in %s on line %d

Warning: Class "MYCLASS" not found in %s on line %d
done
