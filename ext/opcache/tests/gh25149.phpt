--TEST--
GH-25149 (Memory leak of class constant type names in classes not cached by opcache)
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
--FILE--
<?php

eval('class A { const ?GH25149_B C = null; }');
eval('class D { const GH25149_E|GH25149_F|null G = null; }');
var_dump(A::C, D::G);

eval('trait T { const ?GH25149_H I = null; const GH25149_J|GH25149_K|null L = null; }');
eval('class M { use T; }');
var_dump(M::I, M::L);

eval('class N {}');
class O extends N { const ?GH25149_P Q = null; const GH25149_R|GH25149_S|null U = null; }
var_dump(O::Q, O::U);

?>
--EXPECT--
NULL
NULL
NULL
NULL
NULL
NULL
