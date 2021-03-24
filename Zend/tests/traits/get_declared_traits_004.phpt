--TEST--
Testing get_declared_traits() and class_alias()
--FILE--
<?php

trait T { }
class_alias("T", "A");
foreach (get_declared_traits() as $name) {
	if (strlen($name) == 1) {
		echo $name;
	}
}
echo "\n";
?>
--EXPECT--
Ta
