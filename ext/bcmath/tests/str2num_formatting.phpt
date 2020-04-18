--TEST--
bcmath lib arguments formatting
--DESCRIPTION--
1 and 2 argument of bcadd/bcsub/bcmul/bcdiv/bcmod/bcpowmod/bcpow/bccomp (last one works different then others internally);
1 argument of bcsqrt
All of the name above must be well-formed
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
echo bcadd("1", "2"),"\n";
echo bcadd("1.1", "2", 2),"\n";
echo bcadd("", "2", 2),"\n";
echo bcadd("+0", "2"), "\n";
echo bcadd("-0", "2"), "\n";

echo bcadd(" 0", "2");
echo bcadd("1e1", "2");
echo bcadd("1,1", "2");
echo bcadd("Hello", "2");
echo bcadd("1 1", "2");
echo "\n", "\n";

echo bccomp("1", "2"),"\n";
echo bccomp("1.1", "2", 2),"\n";
echo bccomp("", "2"),"\n";
echo bccomp("+0", "2"), "\n";
echo bccomp("-0", "2"), "\n";

echo bccomp(" 0", "2");
echo bccomp("1e1", "2");
echo bccomp("1,1", "2");
echo bccomp("Hello", "2");
echo bccomp("1 1", "2");
?>
--EXPECTF--
3
3.10
2.00
2
2

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
2
Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
2
Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
2
Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
2
Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
2

-1
-1
-1
-1
-1

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
-1
Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
-1
Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
-1
Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
-1
Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
-1