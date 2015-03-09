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
}

$rc = new ReflectionClass("C");
$StaticFlag = 0x01;
$pubFlag =  0x100;
$privFlag = 0x400;

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
?>
--EXPECTF--
No properties:array(0) {
}
Public properties:array(4) {
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
Public or static properties:array(6) {
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