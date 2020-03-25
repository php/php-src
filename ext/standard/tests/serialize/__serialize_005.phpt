--TEST--
__serialize() mechanism (005): parent::__unserialize() is safe
--FILE--
<?php

class A {
    private $data;
    public function __construct(array $data) {
        $this->data = $data;
    }
    public function __serialize() {
        return $this->data;
    }
    public function __unserialize(array $data) {
        $this->data = $data;
    }
}

class B extends A {
    private $data2;
    public function __construct(array $data, array $data2) {
        parent::__construct($data);
        $this->data2 = $data2;
    }
    public function __serialize() {
        return [$this->data2, parent::__serialize()];
    }
    public function __unserialize(array $payload) {
        [$data2, $data] = $payload;
        parent::__unserialize($data);
        $this->data2 = $data2;
    }
}

$common = new stdClass;
$obj = new B([$common], [$common]);
var_dump($s = serialize($obj));
var_dump(unserialize($s));

?>
--EXPECT--
string(63) "O:1:"B":2:{i:0;a:1:{i:0;O:8:"stdClass":0:{}}i:1;a:1:{i:0;r:3;}}"
object(B)#3 (2) {
  ["data2":"B":private]=>
  array(1) {
    [0]=>
    object(stdClass)#4 (0) {
    }
  }
  ["data":"A":private]=>
  array(1) {
    [0]=>
    object(stdClass)#4 (0) {
    }
  }
}
