--TEST--
SPL: FixedArray: overriden iterator methods
--FILE--
<?php
class SplFixedArray2 extends SplFixedArray {
    public function rewind() {
        echo "rewind\n";
        return parent::rewind();
    }
    public function valid() {
        echo "valid\n";
        return parent::valid();
    }
    public function next() {
        echo "next\n";
        return parent::next();
    }
    public function current() {
        echo "current\n";
        return parent::current();
    }
    public function key() {
        echo "key\n";
        return parent::key();
    }
}

$fa = new SplFixedArray2(3);
foreach($fa as $k=>$v) {
    echo "$k=>";
    var_dump($v);
}
?>
--EXPECT--
rewind
valid
current
key
0=>NULL
next
valid
current
key
1=>NULL
next
valid
current
key
2=>NULL
next
valid
