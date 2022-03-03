--TEST--
Bug #62059: ArrayObject and isset are not friends
--FILE--
<?php

class MyArrayObject1 extends ArrayObject {
    public function offsetGet($name): mixed {
        echo "offsetGet($name)\n";
        return parent::offsetGet($name);
    }
    public function offsetExists($name): bool {
        echo "offsetExists($name)\n";
        return parent::offsetExists($name);
    }
}
class MyArrayObject2 extends ArrayObject {
    public function offsetGet($name): mixed {
        echo "offsetGet($name)\n";
        return parent::offsetGet($name);
    }
}
class MyArrayObject3 extends ArrayObject {
    public function offsetExists($name): bool {
        echo "offsetExists($name)\n";
        return parent::offsetExists($name);
    }
}

$arr = [1 => [1 => 42]];
$ao = new ArrayObject($arr);
var_dump(isset($ao[0][1]));
var_dump(isset($ao[1][0]));
var_dump(isset($ao[1][1]));
$ao = new MyArrayObject1($arr);
var_dump(isset($ao[0][1]));
var_dump(isset($ao[1][0]));
var_dump(isset($ao[1][1]));
$ao = new MyArrayObject2($arr);
var_dump(isset($ao[0][1]));
var_dump(isset($ao[1][0]));
var_dump(isset($ao[1][1]));
$ao = new MyArrayObject3($arr);
var_dump(isset($ao[0][1]));
var_dump(isset($ao[1][0]));
var_dump(isset($ao[1][1]));

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
offsetExists(0)
bool(false)
offsetExists(1)
offsetGet(1)
bool(false)
offsetExists(1)
offsetGet(1)
bool(true)
bool(false)
offsetGet(1)
bool(false)
offsetGet(1)
bool(true)
offsetExists(0)
bool(false)
offsetExists(1)
bool(false)
offsetExists(1)
bool(true)
