--TEST--
debug_backtrace() shows scope function frame with correct args
--FILE--
<?php
function test() {
    $fn = fn($a, $b) {
        $trace = debug_backtrace(0);
        // Frame 0 is the scope function itself
        echo $trace[0]['function'] . "\n";
        echo isset($trace[0]['args']) ? count($trace[0]['args']) : 'no args', "\n";
        // Frame 1 is the caller (test)
        echo $trace[1]['function'] . "\n";
    };
    $fn(10, 20);
}
test();
?>
--EXPECTF--
{closure:%s}
2
test
