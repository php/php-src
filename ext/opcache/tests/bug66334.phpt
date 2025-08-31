--TEST--
Bug #66334 (Memory Leak in new pass1_5.c optimizations)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
enable_dl=0
--EXTENSIONS--
opcache
--FILE--
<?php
if (extension_loaded("unknown_extension")) {
    var_dump(1);
} else {
    var_dump(2);
}
?>
--EXPECT--
int(2)
