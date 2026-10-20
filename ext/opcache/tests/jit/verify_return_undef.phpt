--TEST--
VERIFY_RETURN with undef var
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php

function test(): int {
    return $undef;
}

try {
    test();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Undefined variable $undef in %s on line %d
TypeError: test(): Return value must be of type int, null returned
