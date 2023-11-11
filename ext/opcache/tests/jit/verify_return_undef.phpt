--TEST--
VERIFY_RETURN with undef var
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php

function test(): int {
    return $undef;
}

try {
    test();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Undefined variable $undef in %s on line %d
test(): Return value must be of type int, null returned
