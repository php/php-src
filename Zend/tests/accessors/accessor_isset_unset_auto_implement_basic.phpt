--TEST--
ZE2 Tests that isset/unset automatic implementations work as expected, also ensures that isset/unset call the getter/setter accessors
--FILE--
<?php

class AccessorTest {
	public $b {
		get { echo "Getting \$b\n"; return $this->b; }
		set { echo "Setting \$b\n"; $this->b = $value; }
		isset;
		unset;
	}
}

$o = new AccessorTest();
$o->b = 3600;

echo "\$o->b: ".$o->b."\n";
echo "is_null(\$o->b): ".((int)is_null($o->b))."\n";
echo "isset(\$o->b): ".((int)isset($o->b))."\n";
echo "Unsetting \$o->b\n";
unset($o->b);
echo "is_null(\$o->b): ".((int)is_null($o->b))."\n";
echo "isset(\$o->b): ".((int)isset($o->b))."\n";
echo "empty(\$o->b): ".((int)empty($o->b))."\n";
?>
===DONE===
--EXPECTF--
Setting $b
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
Getting $b
empty($o->b): 1
===DONE===
