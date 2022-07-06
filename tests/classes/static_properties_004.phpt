--TEST--
Inherited static properties cannot be separated from their reference set.
--FILE--
<?php
class C { public static $p = 'original'; }
class D extends C {	}
class E extends D {	}

echo "\nInherited static properties refer to the same value across classes:\n";
var_dump(C::$p, D::$p, E::$p);

echo "\nChanging one changes all the others:\n";
D::$p = 'changed.all';
var_dump(C::$p, D::$p, E::$p);

echo "\nReferences cannot be used to split the properties:\n";
$ref = 'changed.one';
D::$p =& $ref;
var_dump(C::$p, D::$p, E::$p);
?>
--EXPECT--
Inherited static properties refer to the same value across classes:
string(8) "original"
string(8) "original"
string(8) "original"

Changing one changes all the others:
string(11) "changed.all"
string(11) "changed.all"
string(11) "changed.all"

References cannot be used to split the properties:
string(11) "changed.one"
string(11) "changed.one"
string(11) "changed.one"
