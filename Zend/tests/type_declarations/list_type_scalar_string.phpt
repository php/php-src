--TEST--
list<string> parameter accepts strings and rejects non-string elements
--FILE--
<?php
declare(strict_types=1);
function f(list<string> $items): void {
    echo count($items), "\n";
}

f(["a", "b"]);

try {
    f(["a", 123]);
} catch (TypeError $e) {
    echo "TypeError\n";
}
?>
--EXPECT--
2
TypeError
