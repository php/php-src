--TEST--
Bug #46064 (Exception when creating ReflectionProperty object on dynamically created property)
--FILE--
<?php

#[AllowDynamicProperties]
class x {
    public $zzz = 2;
}

$o = new x;
$o->z = 1000;
$o->zzz = 3;

var_dump($h = new reflectionproperty($o, 'z'));
var_dump($h->isDefault());
var_dump($h->isPublic());
var_dump($h->isStatic());
var_dump($h->getName());
var_dump(Reflection::getModifierNames($h->getModifiers()));
var_dump($h->getValue($o));

print "---------------------------\n";
try {
    var_dump(new reflectionproperty($o, 'zz'));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

var_dump(new reflectionproperty($o, 'zzz'));

class test {
    protected $a = 1;
}

#[AllowDynamicProperties]
class bar extends test {
    public function __construct() {
        $this->foobar = 2;
        $this->a = 200;

        $p = new reflectionproperty($this, 'foobar');
        var_dump($p->getValue($this), $p->isDefault(), $p->isPublic());
    }
}

new bar;

?>
--EXPECTF--
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "z"
  ["class"]=>
  string(1) "x"
}
bool(false)
bool(true)
bool(false)
string(1) "z"
array(1) {
  [0]=>
  string(6) "public"
}
int(1000)
---------------------------
string(30) "Property x::$zz does not exist"
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(3) "zzz"
  ["class"]=>
  string(1) "x"
}
int(2)
bool(false)
bool(true)
