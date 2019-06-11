--TEST--
get_mangled_object_vars() function
--FILE--
<?php

class A {
    public $pub = 1;
    protected $prot = 2;
    private $priv = 3;
}
class B extends A {
    private $priv = 4;
}

$obj = new B;
$obj->dyn = 5;
$obj->{"6"} = 6;

var_export(get_mangled_object_vars($obj));
echo "\n";

class AO extends ArrayObject {
    private $priv = 1;
}

$ao = new AO(['x' => 'y']);
$ao->dyn = 2;
var_export(get_mangled_object_vars($ao));
echo "\n";
var_export((array) $ao);
echo "\n";

?>
--EXPECT--
array (
  '' . "\0" . 'B' . "\0" . 'priv' => 4,
  'pub' => 1,
  '' . "\0" . '*' . "\0" . 'prot' => 2,
  '' . "\0" . 'A' . "\0" . 'priv' => 3,
  'dyn' => 5,
  6 => 6,
)
array (
  '' . "\0" . 'AO' . "\0" . 'priv' => 1,
  'dyn' => 2,
)
array (
  'x' => 'y',
)
