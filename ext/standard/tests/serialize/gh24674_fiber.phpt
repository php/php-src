--TEST--
GH-24674 (The context shared with Serializable::unserialize() is fiber-local)
--FILE--
<?php
class S implements Serializable {
    public function serialize() { return ''; }
    public function unserialize($d) {
        $name = $d;
        Fiber::suspend();
        var_dump($name, unserialize('R:1;'));
    }
    public function __serialize(): array { return []; }
    public function __unserialize(array $d): void {}
}

$a = new Fiber(function () { var_dump(unserialize('a:2:{i:0;s:1:"A";i:1;C:1:"S":1:{A}}')); });
$b = new Fiber(function () { var_dump(unserialize('a:2:{i:0;s:1:"B";i:1;C:1:"S":1:{B}}')); });
$a->start();
$b->start();
var_dump(unserialize('R:1;'));
$a->resume();
$b->resume();

$c = new Fiber(function () { unserialize('C:1:"S":1:{C}'); });
$c->start();
unset($c);
var_dump(unserialize('R:1;'));
?>
--EXPECTF--
Warning: unserialize(): Error at offset 4 of 4 bytes in %s on line %d
bool(false)
string(1) "A"
array(2) {
  [0]=>
  string(1) "A"
  [1]=>
  object(S)#%d (0) {
  }
}
array(2) {
  [0]=>
  string(1) "A"
  [1]=>
  object(S)#%d (0) {
  }
}
string(1) "B"
array(2) {
  [0]=>
  string(1) "B"
  [1]=>
  object(S)#%d (0) {
  }
}
array(2) {
  [0]=>
  string(1) "B"
  [1]=>
  object(S)#%d (0) {
  }
}

Warning: unserialize(): Error at offset 4 of 4 bytes in %s on line %d
bool(false)
