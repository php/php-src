--TEST--
__sleep() returning undefined declared typed properties
--FILE--
<?php

class Test {
    public int $pub;
    protected ?int $prot;
    private stdClass $priv;

    public function __construct() {
        unset($this->pub, $this->prot, $this->priv);
    }

    public function __sleep() {
        return ['pub', 'prot', 'priv'];
    }
}

var_dump($s = serialize(new Test));
var_dump(unserialize($s));

?>
--EXPECTF--
Notice: serialize(): "pub" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "prot" returned as member variable from __sleep() but does not exist in %s on line %d

Notice: serialize(): "priv" returned as member variable from __sleep() but does not exist in %s on line %d
string(15) "O:4:"Test":0:{}"
object(Test)#1 (0) {
  ["pub"]=>
  uninitialized(int)
  ["prot":protected]=>
  uninitialized(?int)
  ["priv":"Test":private]=>
  uninitialized(stdClass)
}
