--TEST--
__sleep() returning undefined declared properties
--FILE--
<?php

class Test {
    public $pub;
    protected $prot;
    private $priv;

    public function __construct() {
        unset($this->pub, $this->prot, $this->priv);
    }

    public function __sleep() {
        return ['pub', 'prot', 'priv'];
    }
}

var_dump(serialize(new Test));

?>
--EXPECTF--
Warning: serialize(): "pub" returned as member variable from __sleep() but does not exist in %s on line %d

Warning: serialize(): "prot" returned as member variable from __sleep() but does not exist in %s on line %d

Warning: serialize(): "priv" returned as member variable from __sleep() but does not exist in %s on line %d
string(15) "O:4:"Test":0:{}"
