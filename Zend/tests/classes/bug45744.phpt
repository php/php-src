--TEST--
Bug #45744 (Case sensitive callback behaviour)
--FILE--
<?php
class Foo {
    public function __construct(array $data) {
        var_dump(array_map(array($this, 'callback'), $data));
    }

    private function callback($value) {
        if (!is_array($value)) {
            return stripslashes($value);
        }
    return array_map(array($this, 'callback'), $value);
    }
}

class Bar extends Foo {
}

new Bar(array());

class Foo2 {
    public function __construct(array $data) {
        var_dump(array_map(array($this, 'callBack'), $data));
    }

    private function callBack($value) {
    }
}

class Bar2 extends Foo2 {
}

new Bar2(array());
?>
--EXPECT--
array(0) {
}
array(0) {
}
