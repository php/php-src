--TEST--
Test case where the implicit previous finally exception would result in recursion
--FILE--
<?php
try {
    $e = new Exception("M1");
    try {
        throw new Exception("M2", 0, $e);
    } finally {
        throw $e;
    }
} finally {}
?>
--EXPECTF--
Fatal error: Uncaught Exception: M1 in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
