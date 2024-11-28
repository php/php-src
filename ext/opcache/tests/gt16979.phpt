--TEST--
GH-16979: opcache_is_script_cached support file_cache argument
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=0
opcache.file_cache={TMP}
opcache.file_cache_only=1
opcache.log_verbosity_level=4
--EXTENSIONS--
opcache
--FILE--
<?php

opcache_compile_file(__FILE__);

var_dump(
    opcache_is_script_cached(__FILE__, false)
);

var_dump(
    opcache_is_script_cached(__FILE__, true)
);

?>
--EXPECT--
bool(false)
bool(true)
