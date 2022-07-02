--TEST--
SCCP 027: Support for ASSIGN_OBJ_REF
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php
class Foo {
    protected $arr;
    public function init($a) {
        $this->arr =& $a;
        if (isset($a[0])) {
            return 1;
        } else {
            return 2;
        }
    }
}
$x = new Foo();
var_dump($x->init([1]));
?>
--EXPECT--
int(1)
