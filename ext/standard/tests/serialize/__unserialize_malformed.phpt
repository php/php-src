--TEST--
__unserialize() and destructors are not called for incomplete objects
--FILE--
<?php
class Test {
    public function __unserialize(array $data): void {
        echo "__unserialize: ";
        var_dump($data);
    }

    public function __destruct() {
        echo "__destruct\n";
    }
}

foreach ([
    'empty' => 'O:4:"Test":0:{',
    'nonempty' => 'O:4:"Test":1:{i:0;i:42;',
    'nested' => 'a:1:{i:0;O:4:"Test":0:{',
] as $name => $data) {
    echo "$name: ";
    var_dump(@unserialize($data));
}

echo "valid: ";
var_dump(unserialize('O:4:"Test":1:{i:0;i:42;}'));
?>
--EXPECT--
empty: bool(false)
nonempty: bool(false)
nested: bool(false)
valid: __unserialize: array(1) {
  [0]=>
  int(42)
}
object(Test)#1 (0) {
}
__destruct
