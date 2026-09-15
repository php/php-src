--TEST--
GH-17773 (Uncaught exception from by-value function result used by reference)
--FILE--
<?php
function get_value() {
    return 42;
}
set_error_handler(function (int $type, string $message) {
    throw new Exception($message);
});
$array = [&get_value()];
echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Exception: Only variables should be assigned by reference in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(8, 'Only variables ...', '%s', 8)
#1 {main}
  thrown in %s on line %d
