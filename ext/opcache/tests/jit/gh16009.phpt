--TEST--
GH-16009 (Segmentation fault with frameless functions and undefined CVs)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1012
--FILE--
<?php
function testMin2Second(): int {
    $value = min(100, $value);
    return $value;
}
testMin2Second();
?>
--EXPECTF--
Warning: Undefined variable $value in %s on line %d

Fatal error: Uncaught TypeError: testMin2Second(): Return value must be of type int, null returned in %s:%d
Stack trace:
#0 %s(%d): testMin2Second()
#1 {main}
  thrown in %s on line %d
