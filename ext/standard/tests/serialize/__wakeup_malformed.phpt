--TEST--
__wakeup() and destructors are not called for incomplete objects
--FILE--
<?php
class Test {
    public int $value = 0;

    public function __wakeup(): void {
        echo "__wakeup: $this->value\n";
    }

    public function __destruct() {
        echo "__destruct\n";
    }
}

foreach ([
    'empty' => 'O:4:"Test":0:{',
    'nonempty' => 'O:4:"Test":1:{s:5:"value";i:42;',
    'nested' => 'a:1:{i:0;O:4:"Test":0:{',
] as $name => $data) {
    echo "$name: ";
    var_dump(@unserialize($data));
}

echo "valid: ";
var_dump(unserialize('O:4:"Test":1:{s:5:"value";i:42;}'));
?>
--EXPECT--
empty: bool(false)
nonempty: bool(false)
nested: bool(false)
valid: __wakeup: 42
object(Test)#1 (1) {
  ["value"]=>
  int(42)
}
__destruct
