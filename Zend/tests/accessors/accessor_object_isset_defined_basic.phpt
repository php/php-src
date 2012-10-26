--TEST--
ZE2 Tests that an defined isset has its code executed and the return value passed back as the result of the isset() call.
--FILE--
<?php

class AccessorTest {
	public $b {
		get;
		set;
		isset {
			echo "\$b::isset Called.\n";
			return $this->b != NULL;
		}
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
$b::isset Called.
isset($o->b) = 0
$o->b = 10;
$b::isset Called.
isset($o->b) = 1
$o->b = NULL;
$b::isset Called.
isset($o->b) = 0
Done