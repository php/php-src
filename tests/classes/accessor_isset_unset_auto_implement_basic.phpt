--TEST--
ZE2 Tests that isset/unset automatic implementations work as expected
--FILE--
<?php

class AccessorTest {
	public $a = 3600;
	
	public $b {
		get { return $this->a; }
		set { $this->b = $value; }
		_isset;
		_unset;
	}
}

$o = new AccessorTest();

echo "\$o->b: ".$o->b."\n";
echo "is_null(\$o->b): ".((int)is_null($o->b))."\n";
echo "isset(\$o->b): ".((int)isset($o->b))."\n";
unset($o->b);
echo "Unsetting \$o->b\n";
echo "is_null(\$o->b): ".((int)is_null($o->b))."\n";
echo "isset(\$o->b): ".((int)isset($o->b))."\n";
echo "Done\n";
?>
--EXPECTF--
3600
is_null($o->b): false
isset($o->b): true
Unsetting *o->b
is_null($o->b): true
isset($o->b): false
Done