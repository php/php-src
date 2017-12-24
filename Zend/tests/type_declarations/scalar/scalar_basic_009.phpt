--TEST--
nullable scalar parameter type contravariance
--FILE--
<?php

interface I {
    function f1(?bool   $p);
    function f2(?float  $p);
    function f3(?int    $p);
    function f4(?string $p);
}

class C implements I {
    function f1(?scalar $p) { var_dump($p); }
    function f2(?scalar $p) { var_dump($p); }
    function f3(?scalar $p) { var_dump($p); }
    function f4(?scalar $p) { var_dump($p); }
}

$c = new C;

$c->f1(null);
$c->f2(null);
$c->f3(null);
$c->f4(null);

?>
--EXPECT--
NULL
NULL
NULL
NULL
