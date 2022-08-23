--TEST--
load_mangled_object_vars() function
--FILE--
<?php

#[AllowDynamicProperties]
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

$vars = get_mangled_object_vars($obj);

$obj2 = new B;
$obj2->pub = 7;
load_mangled_object_vars($obj2, $vars);

$vars2 = (array) $obj2;
var_dump($vars2 === $vars);

#[AllowDynamicProperties]
class AO extends ArrayObject {
    private $priv = 1;
}

$ao = new AO(['x' => 'y']);
$ao->dyn = 2;
$vars = get_mangled_object_vars($ao);

$ao2 = new AO();
load_mangled_object_vars($ao2, $vars);

$vars2 = get_mangled_object_vars($ao2);
var_dump($vars2 === $vars);

class RO {
    public function __construct(public readonly int $ro = 123)
    {
    }
}

$ro = new RO;
load_mangled_object_vars($ro, []);
var_dump($ro);

load_mangled_object_vars($ro, ['ro' => 'abc']);
var_dump($ro);

#[AllowDynamicProperties]
class DYN {
    public function __set($name, $value)
    {
        $this->data[$name] = $value;
    }
}

$dyn = new DYN;
load_mangled_object_vars($dyn, ['abc' => 123]);
var_dump((array) $dyn);

?>
--EXPECTF--
bool(true)
bool(true)
object(RO)#%d (1) {
  ["ro"]=>
  int(123)
}
object(RO)#%d (1) {
  ["ro"]=>
  int(234)
}
array(1) {
  ["abc"]=>
  int(123)
}
