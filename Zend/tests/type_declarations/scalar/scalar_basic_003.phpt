--TEST--
scalar parameter type contravariance
--FILE--
<?php

interface I {
    function f1(bool   $p);
    function f2(float  $p);
    function f3(int    $p);
    function f4(string $p);
}

class C implements I {
    function f1(scalar $p) { var_dump($p); }
    function f2(scalar $p) { var_dump($p); }
    function f3(scalar $p) { var_dump($p); }
    function f4(scalar $p) { var_dump($p); }
}

$c = new C;
$c->f1('bool');
$c->f2('float');
$c->f3('int');
$c->f4('string');

?>
--EXPECT--
string(4) "bool"
string(5) "float"
string(3) "int"
string(6) "string"
