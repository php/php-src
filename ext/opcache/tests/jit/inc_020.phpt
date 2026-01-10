--TEST--
JIT INC: 020
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo($row) {
    foreach ($row as $key => $value) {
        if (is_int($key)) {
            $key++;
        }
        if (isset($row[$key])) {
            return false;
        }
    }
    return true;
}
?>
OK
--EXPECT--
OK