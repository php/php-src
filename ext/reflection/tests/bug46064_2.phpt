--TEST--
Bug #46064.2 (Exception when creating ReflectionProperty object on dynamicly created property)
--FILE--
<?php

#[AllowDynamicProperties]
class foo {
}

$x = new foo;
$x->test = 2000;


$p = new ReflectionObject($x);
var_dump($p->getProperty('test'));


#[AllowDynamicProperties]
class bar {
    public function __construct() {
        $this->a = 1;
    }
}

class test extends bar {
    private $b = 2;

    public function __construct() {
        parent::__construct();

        $p = new reflectionobject($this);
        var_dump($h = $p->getProperty('a'));
        var_dump($h->isDefault(), $h->isProtected(), $h->isPrivate(), $h->isPublic(), $h->isStatic());
        var_dump($p->getProperties());
    }
}

new test;

?>
--EXPECTF--
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(4) "test"
  ["class"]=>
  string(3) "foo"
}
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(4) "test"
}
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
array(2) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "b"
    ["class"]=>
    string(4) "test"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "a"
    ["class"]=>
    string(4) "test"
  }
}
