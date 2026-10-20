--TEST--
Bug #63734 (Garbage collector can free zvals that are still referenced)
--INI--
zend.enable_gc = 1
--FILE--
<?php
class C {
    public $ref;
    public $ary;
    public function __construct() {
        $this->ref = $this;
        $this->ary[] = 42;
    }
    public function __destruct() {
        global $ary;
        $ary[] = $this->ary[0];
    }
}

$c = new C;
unset($c);
gc_collect_cycles();

var_dump($ary);
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
