--TEST--
GH-17416 (Assertion failure when a throwing error handler runs while an array is unset during foreach)
--FILE--
<?php
function Error2Exception($errno, $errstr, $errfile, $errline) {
    throw new Exception($errstr, $errno);
}
set_error_handler('Error2Exception');
function bar(array &$a): ?bool {
    foreach ($a as $key => $val) {
        if ($val === 2) {
            unset($a[$fusion]);
        }
    }
}
function foo($a, bool $b): bool {
    do {
        $res = bar($a);
    } while ($res === null && $n !== $n2);
}
foo([2, 'a' => 5], false);
?>
--EXPECTF--
Fatal error: Uncaught Exception: Undefined variable $fusion in %s:%d
Stack trace:
#0 %s(%d): Error2Exception(2, 'Undefined varia...', '%s', %d)
#1 %s(%d): bar(Array)
#2 %s(%d): foo(Array, false)
#3 {main}
  thrown in %s on line %d
