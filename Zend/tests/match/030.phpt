--TEST--
Test string match with undefined variable
--FILE--
<?php

set_error_handler(function ($errno, $message) {
    throw new Exception("Custom error handler: $message");
});

echo match ($undefVar) {
   default => "This should not get printed with or without opcache\n",
   '1', '2', '3', '4', '5' => "Also should not be printed\n",
};

echo "unreachable\n";

?>
--EXPECTF--
Fatal error: Uncaught Exception: Custom error handler: Undefined variable $undefVar in %s030.php:4
Stack trace:
#0 %s(%d): {closure:%s:%d}(2, 'Undefined varia...', '%s', 7)
#1 {main}
  thrown in %s030.php on line 4
