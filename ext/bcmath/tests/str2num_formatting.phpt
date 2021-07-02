--TEST--
bcmath lib arguments formatting
--DESCRIPTION--
1 and 2 argument of bcadd/bcsub/bcmul/bcdiv/bcmod/bcpowmod/bcpow/bccomp (last one works different then others internally);
1 argument of bcsqrt
All of the name above must be well-formed
--EXTENSIONS--
bcmath
--FILE--
<?php
echo bcadd("1", "2"),"\n";
echo bcadd("1.1", "2", 2),"\n";
echo bcadd("", "2", 2),"\n";
echo bcadd("+0", "2"), "\n";
echo bcadd("-0", "2"), "\n";

echo "\n";

try {
    echo bcadd(" 0", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    echo bcadd("1e1", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    echo bcadd("1,1", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    echo bcadd("Hello", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    echo bcadd("1 1", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    echo bcadd("1.a", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

echo "\n";

echo bccomp("1", "2"),"\n";
echo bccomp("1.1", "2", 2),"\n";
echo bccomp("", "2"),"\n";
echo bccomp("+0", "2"), "\n";
echo bccomp("-0", "2"), "\n";

echo "\n";

try {
    echo bccomp(" 0", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    echo bccomp("1e1", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    echo bccomp("1,1", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    echo bccomp("Hello", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    echo bccomp("1 1", "2");
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
3
3.10
2.00
2
2

bcadd(): Argument #1 ($num1) is not well-formed
bcadd(): Argument #1 ($num1) is not well-formed
bcadd(): Argument #1 ($num1) is not well-formed
bcadd(): Argument #1 ($num1) is not well-formed
bcadd(): Argument #1 ($num1) is not well-formed
bcadd(): Argument #1 ($num1) is not well-formed

-1
-1
-1
-1
-1

bccomp(): Argument #1 ($num1) is not well-formed
bccomp(): Argument #1 ($num1) is not well-formed
bccomp(): Argument #1 ($num1) is not well-formed
bccomp(): Argument #1 ($num1) is not well-formed
bccomp(): Argument #1 ($num1) is not well-formed
