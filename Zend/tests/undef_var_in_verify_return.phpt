--TEST--
Throwing undef var in verify return
--FILE--
<?php

set_error_handler(function(int $severity, string $message, string $filename, int $lineNumber): void {
    throw new ErrorException($message, 0, $severity, $filename, $lineNumber);
});

function test(): string {
    return $test;
}

test();

?>
--EXPECTF--
Fatal error: Uncaught ErrorException: Undefined variable $test in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(2, 'Undefined varia...', '%s', 8)
#1 %s(%d): test()
#2 {main}
  thrown in %s on line %d
