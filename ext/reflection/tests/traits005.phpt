--TEST--
ReflectionClass::getTraitAlias
--FILE--
<?php
trait T1 { function m1() { } function m2() { } }

class C1 { }
class C2 { use T1; }
class C3 { use T1 { m1 as a1; } }
class C4 { use T1 { m1 as a1; m2 as a2; } }

for ($c = "C1"; $c <= "C4"; $c++) {
    echo "class $c:\n";
    $r = new ReflectionClass($c);
    var_dump($r->getTraitAliases());
    echo "\n";
}
?>
--EXPECT--
class C1:
array(0) {
}

class C2:
array(0) {
}

class C3:
array(1) {
  ["a1"]=>
  string(6) "T1::m1"
}

class C4:
array(2) {
  ["a1"]=>
  string(6) "T1::m1"
  ["a2"]=>
  string(6) "T1::m2"
}
