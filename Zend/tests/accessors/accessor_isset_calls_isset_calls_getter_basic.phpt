--TEST--
ZE2 Tests that the underlying isset() routines calls a getter if one is defined. 
--FILE--
<?php

class AccessorTest {
	public $b {
		get { echo "Getting \$b\n"; return $this->b; }
		set { echo "Setting \$b\n"; $this->b = $value; }
		isset { return isset($this->b); }
	}
}

$o = new AccessorTest();
$o->b = 3600;

echo "\$o->b: ".$o->b."\n";
echo "isset(\$o->b): ".((int)isset($o->b))."\n";
?>
===DONE===
--EXPECTF--
Setting $b
Getting $b
$o->b: 3600
Getting $b
isset($o->b): 1
===DONE===
