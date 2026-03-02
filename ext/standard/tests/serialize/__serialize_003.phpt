--TEST--
__serialize() mechanism (003): Interoperability of different serialization mechanisms
--FILE--
<?php

class Test implements Serializable {
    public function __sleep() {
        echo "__sleep() called\n";
    }

    public function __wakeup() {
        echo "__wakeup() called\n";
    }

    public function __serialize() {
        echo "__serialize() called\n";
        return ["key" => "value"];
    }

    public function __unserialize(array $data) {
        echo "__unserialize() called\n";
        var_dump($data);
    }

    public function serialize() {
        echo "serialize() called\n";
        return "payload";
    }

    public function unserialize($payload) {
        echo "unserialize() called\n";
        var_dump($payload);
    }
}

$test = new Test;
var_dump($s = serialize($test));
var_dump(unserialize($s));

var_dump(unserialize('C:4:"Test":7:{payload}'));

?>
--EXPECT--
__serialize() called
string(37) "O:4:"Test":1:{s:3:"key";s:5:"value";}"
__unserialize() called
array(1) {
  ["key"]=>
  string(5) "value"
}
object(Test)#2 (0) {
}
unserialize() called
string(7) "payload"
object(Test)#2 (0) {
}
