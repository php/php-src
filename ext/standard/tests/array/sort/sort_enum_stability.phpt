--TEST--
SORT_REGULAR produces stable ordering for enums regardless of access order
--FILE--
<?php
enum UnitEnumExample {
    case Hearts;
    case Spades;
    case Clubs;
    case Diamonds;
}

enum BackedEnumExample: string {
    case Alpha = 'alpha';
    case Beta = 'beta';
    case Gamma = 'gamma';
    case Delta = 'delta';
}

function build_cases(string $enumClass, array $order): array {
    $cases = [];
    foreach ($order as $case) {
        $cases[] = constant($enumClass . "::$case");
    }
    return $cases;
}

function sorted_unit(array $order): array {
    $cases = build_cases(UnitEnumExample::class, $order);
    sort($cases, SORT_REGULAR);
    return array_map(fn(UnitEnumExample $c) => $c->name, $cases);
}

function sorted_backed(array $order): array {
    $cases = build_cases(BackedEnumExample::class, $order);
    sort($cases, SORT_REGULAR);
    return array_map(fn(BackedEnumExample $c) => $c->value, $cases);
}

$unitOrders = [
    ['Hearts', 'Spades', 'Clubs', 'Diamonds'],
    ['Diamonds', 'Clubs', 'Spades', 'Hearts'],
    ['Spades', 'Hearts', 'Diamonds', 'Clubs'],
];

$unitBaseline = sorted_unit($unitOrders[0]);
foreach ($unitOrders as $idx => $order) {
    if (sorted_unit($order) !== $unitBaseline) {
        echo "Unit enum order mismatch for permutation $idx\n";
    }
}

$backedOrders = [
    ['Alpha', 'Beta', 'Gamma', 'Delta'],
    ['Delta', 'Gamma', 'Beta', 'Alpha'],
    ['Beta', 'Alpha', 'Delta', 'Gamma'],
];

$backedBaseline = sorted_backed($backedOrders[0]);
foreach ($backedOrders as $idx => $order) {
    if (sorted_backed($order) !== $backedBaseline) {
        echo "Backed enum order mismatch for permutation $idx\n";
    }
}

echo "done\n";
?>
--EXPECT--
done
