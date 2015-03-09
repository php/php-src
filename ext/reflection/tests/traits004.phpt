--TEST--
ReflectionClass::getTraits() and ReflectionClass::getTraitNames
--FILE--
<?php
trait T1 { }
trait T2 { }

class C1 { }
class C2 { use T1; }
class C3 { use T1; use T2; }

for ($c = "C1"; $c <= "C3"; $c++) {
    echo "class $c:\n";
    $r = new ReflectionClass($c);
    var_dump($r->getTraitNames());
    var_dump($r->getTraits());
    echo "\n";
}
--EXPECT--
class C1:
array(0) {
}
array(0) {
}

class C2:
array(1) {
  [0]=>
  string(2) "T1"
}
array(1) {
  ["T1"]=>
  object(ReflectionClass)#1 (1) {
    ["name"]=>
    string(2) "T1"
  }
}

class C3:
array(2) {
  [0]=>
  string(2) "T1"
  [1]=>
  string(2) "T2"
}
array(2) {
  ["T1"]=>
  object(ReflectionClass)#2 (1) {
    ["name"]=>
    string(2) "T1"
  }
  ["T2"]=>
  object(ReflectionClass)#3 (1) {
    ["name"]=>
    string(2) "T2"
  }
}

