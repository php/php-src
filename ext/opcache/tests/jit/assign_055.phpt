--TEST--
JIT ASSIGN: memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--FILE--
<?php
set_error_handler(function() {
    (y);
});
$ret = new stdClass;
try {
    $ret = $y;
} catch (y) {
}
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %sassign_055.php:3
Stack trace:
#0 %sassign_055.php(7): {closure}(2, 'Undefined varia...', '%s', 7)
#1 {main}
  thrown in %sassign_055.php on line 3