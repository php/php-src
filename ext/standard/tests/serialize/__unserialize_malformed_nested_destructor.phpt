--TEST--
Destructor of a nested object in a failed __unserialize() payload cannot reach freed slots
--FILE--
<?php
class T {
    public function __unserialize(array $data): void {}
}

class D {
    public function __destruct() {
        var_dump(unserialize('R:2;'));
    }
}

foreach ([
    'missing brace' => 'O:1:"T":2:{i:0;s:5:"hello";i:1;O:1:"D":0:{}',
    'malformed element' => 'O:1:"T":3:{i:0;s:5:"hello";i:1;O:1:"D":0:{}i:2;X',
] as $name => $data) {
    echo "$name: ";
    var_dump(@unserialize($data));
}
?>
--EXPECT--
missing brace: bool(false)
bool(false)
malformed element: bool(false)
bool(false)
