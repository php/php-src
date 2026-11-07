--TEST--
GH-24674 (C: payload unserialized from __wakeup() or __unserialize())
--FILE--
<?php
class A {}

class S implements Serializable {
    public $data;
    public function serialize() { return serialize($this->data); }
    public function unserialize($data) { $this->data = unserialize($data); }
}

function check($where) {
    $p = 'x:i:0;a:1:{i:0;O:1:"A":0:{}};m:a:0:{}';
    $r = unserialize('C:11:"ArrayObject":' . strlen($p) . ':{' . $p . '}', ['allowed_classes' => ['ArrayObject']]);
    echo "$where allowed_classes: ", get_class($r[0]), "\n";

    $o = new stdClass;
    $s = new S;
    $s->data = [$o, $o];
    $r = unserialize(serialize($s));
    echo "$where round trip: ";
    var_dump($r->data[0] === $r->data[1]);
}

class W {
    public function __wakeup() { check('__wakeup'); }
}

class U {
    public function __unserialize(array $data): void { check('__unserialize'); }
}

unserialize('O:1:"W":0:{}');
unserialize('O:1:"U":0:{}');
check('top level');
?>
--EXPECTF--
Deprecated: %s Serializable interface, which is deprecated. %s
__wakeup allowed_classes: __PHP_Incomplete_Class
__wakeup round trip: bool(true)
__unserialize allowed_classes: __PHP_Incomplete_Class
__unserialize round trip: bool(true)
top level allowed_classes: __PHP_Incomplete_Class
top level round trip: bool(true)
