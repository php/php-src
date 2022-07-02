--TEST--
Bug #71731: Null coalescing operator and ArrayAccess
--FILE--
<?php

class AA implements ArrayAccess {
    private $data = [];
    public function offsetExists($name): bool {
        echo "offsetExists($name)\n";
        return array_key_exists($name, $this->data);
    }
    public function &offsetGet($name): mixed {
        echo "offsetGet($name)\n";
        if (!array_key_exists($name, $this->data)) {
            throw new Exception('Unknown offset');
        }
        return $this->data[$name];
    }
    public function offsetSet($name, $value): void {
        echo "offsetSet($name)\n";
        $this->data[$name] = $value;
    }
    public function offsetUnset($name): void {
        echo "offsetUnset($name)\n";
        unset($this->data[$name]);
    }
}

$aa = new AA;
var_dump(isset($aa[0][1][2]));
var_dump(isset($aa[0]->foo));
var_dump($aa[0] ?? 42);
var_dump($aa[0][1][2] ?? 42);

$aa[0] = new AA;
$aa[0][1] = new AA;
var_dump(isset($aa[0][1][2]));
var_dump($aa[0][1][2] ?? 42);

?>
--EXPECT--
offsetExists(0)
bool(false)
offsetExists(0)
bool(false)
offsetExists(0)
int(42)
offsetExists(0)
int(42)
offsetSet(0)
offsetGet(0)
offsetSet(1)
offsetExists(0)
offsetGet(0)
offsetExists(1)
offsetGet(1)
offsetExists(2)
bool(false)
offsetExists(0)
offsetGet(0)
offsetExists(1)
offsetGet(1)
offsetExists(2)
int(42)
