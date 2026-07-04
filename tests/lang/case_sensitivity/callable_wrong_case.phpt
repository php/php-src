--TEST--
Array and string callables with wrong-case class name fails with wrong case
--FILE--
<?php
class MyService {
    public static function process(): string { return "ok"; }
}

// Array callable — wrong-cased class name
$result = call_user_func(["MYSERVICE", "process"]);
echo $result . "\n";

$result2 = call_user_func(["myservice", "process"]);
echo $result2 . "\n";

// String callable — wrong-cased class name
$result3 = call_user_func("MYSERVICE::process");
echo $result3 . "\n";

// Correct — no warning
$result4 = call_user_func(["MyService", "process"]);
echo $result4 . "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, class "MYSERVICE" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
