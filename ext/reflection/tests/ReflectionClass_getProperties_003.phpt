--TEST--
ReflectionClass::getProperties()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
    public  $pub1;
    public  $pub2;
    private  $priv1;
    private  $priv2;
    static public  $pubs;
    static public  $pubs2;
    static private  $privs1;
    static private  $privs2;
    public $hookNoVirt { set { $this->hookNoVirt = strtoupper($value); } }
    public $hookVirt { get { return 42; } }
}

$rc = new ReflectionClass("C");
$StaticFlag = ReflectionProperty::IS_STATIC;
$pubFlag =  ReflectionProperty::IS_PUBLIC;
$privFlag = ReflectionProperty::IS_PRIVATE;
$virtFlag = ReflectionProperty::IS_VIRTUAL;

echo "No properties:";
var_dump($rc->getProperties(0));

echo "Public properties:";
var_dump($rc->getProperties($pubFlag));

echo "Private properties:";
var_dump($rc->getProperties($privFlag));

echo "Public or static properties:";
var_dump($rc->getProperties($StaticFlag | $pubFlag));

echo "Private or static properties:";
var_dump($rc->getProperties($StaticFlag | $privFlag));

echo "Virtual properties:";
var_dump($rc->getProperties($virtFlag));
?>
--EXPECTF--
No properties:array(0) {
}
Public properties:array(6) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(4) "pub1"
    ["class"]=>
    string(1) "C"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(4) "pub2"
    ["class"]=>
    string(1) "C"
  }
  [2]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(4) "pubs"
    ["class"]=>
    string(1) "C"
  }
  [3]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "pubs2"
    ["class"]=>
    string(1) "C"
  }
  [4]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(10) "hookNoVirt"
    ["class"]=>
    string(1) "C"
  }
  [5]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(8) "hookVirt"
    ["class"]=>
    string(1) "C"
  }
}
Private properties:array(4) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "priv1"
    ["class"]=>
    string(1) "C"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "priv2"
    ["class"]=>
    string(1) "C"
  }
  [2]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(6) "privs1"
    ["class"]=>
    string(1) "C"
  }
  [3]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(6) "privs2"
    ["class"]=>
    string(1) "C"
  }
}
Public or static properties:array(8) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(4) "pub1"
    ["class"]=>
    string(1) "C"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(4) "pub2"
    ["class"]=>
    string(1) "C"
  }
  [2]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(4) "pubs"
    ["class"]=>
    string(1) "C"
  }
  [3]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "pubs2"
    ["class"]=>
    string(1) "C"
  }
  [4]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(6) "privs1"
    ["class"]=>
    string(1) "C"
  }
  [5]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(6) "privs2"
    ["class"]=>
    string(1) "C"
  }
  [6]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(10) "hookNoVirt"
    ["class"]=>
    string(1) "C"
  }
  [7]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(8) "hookVirt"
    ["class"]=>
    string(1) "C"
  }
}
Private or static properties:array(6) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "priv1"
    ["class"]=>
    string(1) "C"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "priv2"
    ["class"]=>
    string(1) "C"
  }
  [2]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(4) "pubs"
    ["class"]=>
    string(1) "C"
  }
  [3]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(5) "pubs2"
    ["class"]=>
    string(1) "C"
  }
  [4]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(6) "privs1"
    ["class"]=>
    string(1) "C"
  }
  [5]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(6) "privs2"
    ["class"]=>
    string(1) "C"
  }
}
Virtual properties:array(1) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(8) "hookVirt"
    ["class"]=>
    string(1) "C"
  }
}
