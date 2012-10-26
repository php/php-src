--TEST--
ZE2 Tests that isset/unset default implementations work as expected, also ensures that isset/unset call the getter/setter accessors
--FILE--
<?php

class AccessorTest {
	public $a = 3600;
	
	public $b {
		get { echo "Getting \$b\n"; return $this->a; }
		set { echo "Setting \$b\n"; $this->a = $value; }
	}
}

$o = new AccessorTest();

echo "\$o->b: ".$o->b."\n";
echo "is_null(\$o->b): ".((int)is_null($o->b))."\n";
echo "isset(\$o->b): ".((int)isset($o->b))."\n";
echo "Unsetting \$o->b\n";
unset($o->b);
echo "is_null(\$o->b): ".((int)is_null($o->b))."\n";
echo "isset(\$o->b): ".((int)isset($o->b))."\n";
echo "Done\n";
?>
--EXPECTF--
Getting $b
$o->b: 3600
Getting $b
is_null($o->b): 0
Getting $b
isset($o->b): 1
Unsetting $o->b
Setting $b
Getting $b
is_null($o->b): 1
Getting $b
isset($o->b): 0
Done