--TEST--
GH-24674 (Nested unserialize() shares the context of a parse in progress)
--FILE--
<?php
class P {}

set_error_handler(function ($errno, $errstr) {
    echo "$errstr: ";
    var_dump(@unserialize('R:1;'));
    return true;
});
var_dump(unserialize('O:1:"P":1:{s:3:"dyn";i:1;}') instanceof P);
var_dump(unserialize('a:1:{i:0;O:8:"stdClass":0:{}}junk'));
restore_error_handler();

class N {
    public function __toString(): string {
        echo "__toString: ";
        var_dump(unserialize('a:1:{i:0;s:6:"secret";}'));
        return 'stdClass';
    }
}

var_dump(@unserialize('R:2;', ['allowed_classes' => [new N]]));

class S implements Serializable {
    public $o;
    public function serialize() { return ''; }
    public function unserialize($data) { $this->o = unserialize($data); }
    public function __serialize(): array { return []; }
    public function __unserialize(array $data): void {}
}

$r = unserialize('a:2:{i:0;O:8:"stdClass":0:{}i:1;C:1:"S":4:{r:2;}}');
echo "Serializable back-reference: ";
var_dump($r[1]->o === $r[0]);
?>
--EXPECTF--
Creation of dynamic property P::$dyn is deprecated: bool(false)
bool(true)
unserialize(): Extra data starting at offset 29 of 33 bytes: bool(false)
array(1) {
  [0]=>
  object(stdClass)#%d (0) {
  }
}
__toString: array(1) {
  [0]=>
  string(6) "secret"
}
bool(false)
Serializable back-reference: bool(true)
