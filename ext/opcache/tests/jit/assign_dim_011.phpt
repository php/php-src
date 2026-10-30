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
--EXPECT--
Err: Automatic conversion of false to array is deprecated
Exception: Cannot use a scalar value as an array
DONE
