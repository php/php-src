--TEST--
ZE2 Tests that an undefined unset defaults to the same functionality as an autodefined unset, which is setting the value to NULL
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
unset($o->b);
echo "unset(\$o->b);\n";
echo "isset(\$o->b) = ".(int)isset($o->b)."\n";
echo "Done";
?>
--EXPECTF--
isset($o->b) = 0
$o->b = 10;
isset($o->b) = 1
unset($o->b);
isset($o->b) = 0
Done