--TEST--
Exceptions ignoring arguments
--FILE--
<?php
$function = function(string $user, string $pass) {
    throw new Exception();
};

ini_set("zend.exception_ignore_args", 1);

$function("secrets", "arewrong");
?>
--EXPECTF--
Fatal error: Uncaught Exception in %sexception_ignore_args.php:3
Stack trace:
#0 %s(%d): {closure:%s:%d}()
#1 {main}
  thrown in %sexception_ignore_args.php on line 3
