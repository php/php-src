--TEST--
Bug #71359: Null coalescing operator and magic
--FILE--
<?php
class AA {
    private $data = [];
    public function __isset($name) {
        echo "__isset($name)\n";
        return array_key_exists($name, $this->data);
    }
    public function &__get($name) {
        echo "__get($name)\n";
        if (!array_key_exists($name, $this->data)) {
            throw new Exception('Unknown offset');
        }
        return $this->data[$name];
    }
    public function __set($name, $value) {
        echo "__set($name)\n";
        $this->data[$name] = $value;
    }
    public function __unset($name) {
        echo "__unset($name)\n";
        unset($this->data[$name]);
    }
}

$aa = new AA;
var_dump(isset($aa->zero->one->two));
var_dump(isset($aa->zero->foo));
var_dump($aa->zero ?? 42);
var_dump($aa->zero->one->two ?? 42);
$aa->zero = new AA;
$aa->zero->one = new AA;
var_dump(isset($aa->zero->one->two));
var_dump($aa->zero->one->two ?? 42);
?>
--EXPECT--
__isset(zero)
bool(false)
__isset(zero)
bool(false)
__isset(zero)
int(42)
__isset(zero)
int(42)
__set(zero)
__get(zero)
__set(one)
__isset(zero)
__get(zero)
__isset(one)
__get(one)
__isset(two)
bool(false)
__isset(zero)
__get(zero)
__isset(one)
__get(one)
__isset(two)
int(42)
