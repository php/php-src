--TEST--
Defer with recursive function calls
--FILE--
<?php
function factorial($n, $depth = 0) {
    $indent = str_repeat("  ", $depth);
    echo "{$indent}factorial($n) start\n";

    defer {
        $indent = str_repeat("  ", $depth);
        echo "{$indent}factorial($n) defer\n";
    }

    if ($n <= 1) {
        echo "{$indent}factorial($n) base case\n";
        return 1;
    }

    $result = $n * factorial($n - 1, $depth + 1);
    echo "{$indent}factorial($n) = $result\n";
    return $result;
}

$result = factorial(4);
echo "Final result: $result\n";
?>
--EXPECT--
factorial(4) start
  factorial(3) start
    factorial(2) start
      factorial(1) start
      factorial(1) base case
      factorial(1) defer
    factorial(2) = 2
    factorial(2) defer
  factorial(3) = 6
  factorial(3) defer
factorial(4) = 24
factorial(4) defer
Final result: 24
