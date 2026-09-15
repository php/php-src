--TEST--
GH-17773 (Exception while reporting by-value function result used by reference)
--FILE--
<?php
function get_value() {
    return 42;
}

set_error_handler(function (int $type, string $message) {
    throw new Exception($message);
});

try {
    $array = [&get_value()];
} catch (Exception $exception) {
    echo $exception->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
Only variables should be assigned by reference
Done
