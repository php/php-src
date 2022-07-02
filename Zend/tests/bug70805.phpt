--TEST--
Bug #70805 (Segmentation faults whilst running Drupal 8 test suite)
--FILE--
<?php
class A {
    public $b;
}

class B {
    public $a;
}

class C {
    public function __destruct() {
        if (isset($GLOBALS["a"])) {
            unset($GLOBALS["array"]);
            unset($GLOBALS["a"]); // this will be called in gc_colloct_roots and put $a into gc roots buf
        }
    }
}

$a = new A;
$a->b = new B;
$a->b->a = $a;

$i = 0;

$c = new A;
$array = array($c); //This is used to leave a room for $GLOBALS["a"]
unset($c);

while ($i++ < 9998) {
    $t = [];
    $t[] = &$t;
    unset($t);
}
$t = [new C];
$t[] = &$t;
unset($t); // This is used to trigger C::__destruct while doing gc_colloct_roots

$e = $a;
unset($a); // This one cannot be put into roots buf because it's full, thus gc_colloct_roots will be called,
           // but C::__destructor which is called in gc_colloct_roots will put $a into buf
           // which will make $a be put into gc roots buf twice
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(0)
