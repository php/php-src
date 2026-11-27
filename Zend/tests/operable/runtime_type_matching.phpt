--TEST--
Runtime type matching for Operable methods (strict primitives, unions, intersections, DNF, self/parent, no autoload)
--FILE--
<?php

$autoloadCalls = [];
spl_autoload_register(function (string $class) use (&$autoloadCalls) {
    $autoloadCalls[] = $class;
});

interface A {}
interface B {}
final class OnlyA implements A {}
final class OnlyB implements B {}
final class BothAB implements A, B {}

readonly class BaseOp implements Operable {
    final public function __add(self $other): string {
        return "BaseOp::__add(" . get_class($other) . ")";
    }
}

final readonly class SubOp extends BaseOp {
    public function __sub(parent $other): string {
        return "SubOp::__sub(" . get_class($other) . ")";
    }
}

final readonly class TypeTester implements Operable {
    public function __add(int $x): string {
        return "int:" . get_debug_type($x) . ":$x";
    }
    public function __sub(float $x): string {
        return "float:" . get_debug_type($x) . ":$x";
    }
    public function __mul(self|int $x): string {
        return "self|int:" . get_debug_type($x);
    }
    public function __div(OnlyA|OnlyB|int $x): string {
        return "union:" . get_debug_type($x);
    }
    public function __mod((A&B)|float $x): string {
        return "dnf:" . get_debug_type($x);
    }
    public function __pow(NonExistentClass|int $x): string {
        return "noautoload:" . get_debug_type($x);
    }
}

$t = new TypeTester();

// Strict int: accepts int, rejects float and numeric string
var_dump($t + 7);
foreach ([7.0, "7", true, null, []] as $bad) {
    try {
        $t + $bad;
        echo "UNEXPECTED\n";
    } catch (InvalidOperatorError $e) {
        echo "Rejected + " . get_debug_type($bad) . "\n";
    }
}

// Strict float: accepts float AND int (widened to float), rejects numeric string
var_dump($t - 2.5);
var_dump($t - 2);
try {
    $t - "2.5";
} catch (InvalidOperatorError $e) {
    echo "Rejected - string\n";
}

// Single class + primitive mask: self|int
var_dump($t * $t);
var_dump($t * 3);
try {
    $t * 3.5;
} catch (InvalidOperatorError $e) {
    echo "Rejected * float\n";
}

// Multi-class union + primitive mask: OnlyA|OnlyB|int
var_dump($t / new OnlyA());
var_dump($t / new OnlyB());
var_dump($t / 9);
try {
    $t / new BothAB();
} catch (InvalidOperatorError $e) {
    echo "Rejected / BothAB\n";
}

// DNF + primitive mask: (A&B)|float
var_dump($t % new BothAB());
var_dump($t % 1.5);
var_dump($t % 4);
try {
    $t % new OnlyA();
} catch (InvalidOperatorError $e) {
    echo "Rejected % OnlyA\n";
}

// self and parent resolution
$base = new BaseOp();
$sub = new SubOp();
var_dump($base + $sub);
var_dump($sub + $base);
var_dump($sub - $base);
var_dump($sub - $sub);

// Never triggers autoloader on unloaded class in type hint
var_dump($t ** 2);
try {
    $t ** new stdClass();
} catch (InvalidOperatorError $e) {
    echo "Rejected ** stdClass\n";
}
var_dump($autoloadCalls);

// Class alias and case-insensitive class hint matching (no allocation fallback)
class_alias(OnlyA::class, 'AliasedOnlyA');
final readonly class AliasTester implements Operable {
    public function __add(AliasedOnlyA $x): string {
        return "alias:" . get_class($x);
    }
    public function __sub(onlyb $x): string {
        return "ci:" . get_class($x);
    }
}
$at = new AliasTester();
var_dump($at + new OnlyA());
var_dump($at - new OnlyB());

?>
--EXPECT--
string(9) "int:int:7"
Rejected + float
Rejected + string
Rejected + bool
Rejected + null
Rejected + array
string(15) "float:float:2.5"
string(13) "float:float:2"
Rejected - string
string(19) "self|int:TypeTester"
string(12) "self|int:int"
Rejected * float
string(11) "union:OnlyA"
string(11) "union:OnlyB"
string(9) "union:int"
Rejected / BothAB
string(10) "dnf:BothAB"
string(9) "dnf:float"
string(9) "dnf:float"
Rejected % OnlyA
string(20) "BaseOp::__add(SubOp)"
string(21) "BaseOp::__add(BaseOp)"
string(20) "SubOp::__sub(BaseOp)"
string(19) "SubOp::__sub(SubOp)"
string(14) "noautoload:int"
Rejected ** stdClass
array(0) {
}
string(11) "alias:OnlyA"
string(8) "ci:OnlyB"
