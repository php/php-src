--TEST--
JIT FETCH_DIM_W: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
set_error_handler(function() {
    $GLOBALS['a']='';
});
$a[3E44]='';
?>
DONE
--EXPECT--
DONE
