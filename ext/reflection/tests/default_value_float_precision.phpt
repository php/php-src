--TEST--
Reflection prints float default values with round-trip precision
--INI--
precision=14
--FILE--
<?php

const FOO = 1;

#[Attribute]
class Attr {
    public function __construct(public $x) {}
}

class C {
    public $p = 0.30000000000000004;

    #[Attr(0.30000000000000004)]
    public function m(
        $a = 0.30000000000000004,
        $b = FOO + 0.30000000000000004,
        $c = [0.1, 1.0, 1e100],
        $d = -0.0,
        $e = INF,
    ) {}
}

$m = new ReflectionMethod(C::class, 'm');
foreach ($m->getParameters() as $p) {
    echo $p, "\n";
}
echo new ReflectionProperty(C::class, 'p');
echo $m->getAttributes()[0];

?>
--EXPECT--
Parameter #0 [ <optional> $a = 0.30000000000000004 ]
Parameter #1 [ <optional> $b = FOO + 0.30000000000000004 ]
Parameter #2 [ <optional> $c = [0.1, 1.0, 1.0E+100] ]
Parameter #3 [ <optional> $d = -0.0 ]
Parameter #4 [ <optional> $e = INF ]
Property [ public $p = 0.30000000000000004 ]
Attribute [ Attr ] {
  - Arguments [1] {
    Argument #0 [ 0.30000000000000004 ]
  }
}
