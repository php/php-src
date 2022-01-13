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
  "\000B\000priv" => 4,
  'pub' => 1,
  "\000*\000prot" => 2,
  "\000A\000priv" => 3,
  'dyn' => 5,
  6 => 6,
)
array (
  "\000AO\000priv" => 1,
  'dyn' => 2,
)
array (
  'x' => 'y',
)
