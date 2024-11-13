--TEST--
JIT FETCH_DIM_FUNC_ARG: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
new class(true[""]) {
}
?>
DONE
--EXPECTF--
Warning: Trying to access array offset on true in %sfetch_dim_func_arg_002.php on line 2
DONE
