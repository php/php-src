--TEST--
JIT ASSIGN_DIM: 011
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
$my_var = null < 
set_error_handler(function($code, $msg) use(&$my_var) {
	echo "Err: $msg\n";
    $my_var[] = $my_var = 0;
});
try {
    $my_var[] = "";
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
?>
DONE
--EXPECTF--
Err: Automatic conversion of false to array is deprecated

Fatal error: Uncaught Error: Cannot use a scalar value as an array in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(8192, 'Automatic conve...', '%s', %d)
#1 {main}
  thrown in %s on line %d
