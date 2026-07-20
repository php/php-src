--TEST--
array_intersect_assoc() with empty operands
--FILE--
<?php

echo "Safe empty operands:\n";
var_dump(array_intersect_assoc([], ['k' => []], ['k' => []]));
var_dump(array_intersect_assoc(['k' => []], [], ['k' => []]));

echo "Single operand:\n";
var_dump(array_intersect_assoc(['k' => 1]));
var_dump(array_intersect_assoc([]));

echo "Warnings before third empty operand:\n";
$warnings = [];
set_error_handler(static function (int $severity, string $message) use (&$warnings): bool {
    if ($severity === E_WARNING) {
        $warnings[] = $message;
        return true;
    }
    return false;
});
$result = array_intersect_assoc(['k' => []], ['k' => []], []);
restore_error_handler();
var_dump($result);
var_dump($warnings);

echo "Warning converted to exception:\n";
set_error_handler(static function (int $severity, string $message): never {
    throw new ErrorException($message, 0, $severity);
});
try {
    array_intersect_assoc(['k' => []], ['k' => []], []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
} finally {
    restore_error_handler();
}

echo "Invalid argument after empty operand:\n";
try {
    array_intersect_assoc(['k' => 1], [], 42);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Safe empty operands:
array(0) {
}
array(0) {
}
Single operand:
array(1) {
  ["k"]=>
  int(1)
}
array(0) {
}
Warnings before third empty operand:
array(0) {
}
array(2) {
  [0]=>
  string(26) "Array to string conversion"
  [1]=>
  string(26) "Array to string conversion"
}
Warning converted to exception:
ErrorException: Array to string conversion
Invalid argument after empty operand:
array_intersect_assoc(): Argument #3 must be of type array, int given
