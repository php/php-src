--TEST--
JIT FETCH_DIM_R: 010
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
set_error_handler(function() {
    $GLOBALS['a'] = 0;
});
$a = [$y];
($a[$b]);
?>
DONE
--EXPECT--
DONE
