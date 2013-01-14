--TEST--
Bug #60909 (custom error handler throwing Exception + fatal error = no shutdown function).
--FILE--
<?php
register_shutdown_function(function(){echo("\n\n!!!shutdown!!!\n\n");});
set_error_handler(function($errno, $errstr, $errfile, $errline){
 echo "error($errstr)";
 throw new Exception("Foo");
});

require 'notfound.php';
--EXPECTF--
error(require(notfound.php): failed to open stream: %s)
Warning: Uncaught exception 'Exception' with message 'Foo' in %sbug60909_1.php:5
Stack trace:
#0 %sbug60909_1.php(8): {closure}(2, 'require(notfoun...', '%s', 8, Array)
#1 %sbug60909_1.php(8): require()
#2 {main}
  thrown in %sbug60909_1.php on line 5

Fatal error: main(): Failed opening required 'notfound.php' (include_path='%s') in %sbug60909_1.php on line 8


!!!shutdown!!!
