--TEST--
Bug #72057 (PHP hangs when user error handler throws exception after Notice from type coercion)
--FILE--
<?php

set_error_handler(
    function() {
        throw new Exception("My custom error");
    }
);

(function (int $i) { bar(); })("7as");

--EXPECTF--

Fatal error: Uncaught Exception: My custom error in %s:%d
Stack trace:
#0 %s(%d): {closure}(8, 'A non well form...', '%s', %d, Array)
#1 %s(%d): {closure}('7as')
#2 {main}
  thrown in %s on line %d
