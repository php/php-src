--TEST--
JIT ASSIGN_DIM: 014
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
set_error_handler(function($code, $err) {
    echo "Error: $err\n";
    $GLOBALS['a'] = null;
});
$a[$y] = function(){};
?>
DONE
--EXPECT--
Error: Undefined variable $y
DONE
