--TEST--
SPL: FixedArray: Iterators
--FILE--
<?php
class A extends SplFixedArray {
	public $prop1 = "dummy";
	public $prop2 = "dummy";

    public function current() {
        echo "A::current\n";
        return parent::current();
    }
    public function key() {
        echo "A::key\n";
        return parent::key();
    }
    public function rewind() {
        echo "A::rewind\n";
        return parent::rewind();
    }
    public function valid() {
        echo "A::valid\n";
        return parent::valid();
    }
    public function next() {
        echo "A::next\n";
        return parent::next();
    }
}

echo "==Direct instance==\n";
$a = new SplFixedArray(5);
$a[0] = "a";
$a[1] = "c";
$a[2] = "d";
$a[3] = "e";
$a[4] = "f";
foreach ($a as $k => $v) {
    echo "$k => $v\n";
}
echo "==Child instance==\n";
$a = new A(5);
$a[0] = "a";
$a[1] = "c";
$a[2] = "d";
$a[3] = "e";
$a[4] = "f";
foreach ($a as $k => $v) {
    echo "$k => $v\n";
}
?>
===DONE===
--EXPECT--
==Direct instance==
0 => a
1 => c
2 => d
3 => e
4 => f
==Child instance==
A::rewind
A::valid
A::current
A::key
0 => a
A::next
A::valid
A::current
A::key
1 => c
A::next
A::valid
A::current
A::key
2 => d
A::next
A::valid
A::current
A::key
3 => e
A::next
A::valid
A::current
A::key
4 => f
A::next
A::valid
===DONE===
