--TEST--
PFA inherits NoDiscard and SensitiveParameter attributes
--FILE--
<?php

#[Attribute]
class Test {}

#[NoDiscard] #[Test]
function f($a, #[SensitiveParameter] $b, #[Test] ...$c) {
}

function dump_attributes($function) {
    $r = new ReflectionFunction($function);
    var_dump($r->getAttributes());

    foreach ($r->getParameters() as $i => $p) {
        echo "Parameter $i:\n";
        var_dump($p->getAttributes());
    }
}

echo "# Orig attributes:\n";

dump_attributes('f');

$f = f(1, ?, ?, ...);

echo "# PFA attributes:\n";

dump_attributes($f);

?>
--EXPECTF--
# Orig attributes:
array(2) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(9) "NoDiscard"
  }
  [1]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(4) "Test"
  }
}
Parameter 0:
array(0) {
}
Parameter 1:
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(18) "SensitiveParameter"
  }
}
Parameter 2:
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(4) "Test"
  }
}
# PFA attributes:
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(9) "NoDiscard"
  }
}
Parameter 0:
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(18) "SensitiveParameter"
  }
}
Parameter 1:
array(0) {
}
Parameter 2:
array(0) {
}
