--TEST--
Generic class: enum implementing generic interface
--FILE--
<?php
declare(strict_types=1);

interface Container<T> {
    public function value(): T;
}

// Backed enum implementing generic interface with bound type
enum Color: string implements Container<string> {
    case Red = 'red';
    case Blue = 'blue';
    case Green = 'green';

    public function value(): string {
        return $this->value;
    }
}

// 1. Basic usage
$c = Color::Red;
echo "1. " . $c->value() . "\n";

// 2. instanceof generic interface
echo "2. is Container: " . ($c instanceof Container ? "yes" : "no") . "\n";

// 3. Type hint accepting generic interface
function getFromContainer(Container<string> $c): string {
    return $c->value();
}

echo "3. " . getFromContainer(Color::Blue) . "\n";

// 4. All enum cases
foreach (Color::cases() as $case) {
    echo "4. " . $case->name . "=" . $case->value() . "\n";
}

echo "Done.\n";
?>
--EXPECT--
1. red
2. is Container: yes
3. blue
4. Red=red
4. Blue=blue
4. Green=green
Done.
