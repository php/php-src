--TEST--
ZE2 Tests that an autoimplemented isset defaults to checking the value against a NULL/non-NULL state.
--FILE--
<?php

class AccessorTest {
	public $b {
		get;
		set;
		isset;
	}
}

$o = new AccessorTest();

echo "isset(\$o->b) = ".(int)isset($o->b)."\n";
$o->b = 10;
echo "\$o->b = 10;\n";
echo "isset(\$o->b) = ".(int)isset($o->b)."\n";
$o->b = NULL;
echo "\$o->b = NULL;\n";
echo "isset(\$o->b) = ".(int)isset($o->b)."\n";
echo "Done";
?>
--EXPECTF--
isset($o->b) = 0
$o->b = 10;
isset($o->b) = 1
$o->b = NULL;
isset($o->b) = 0
Done