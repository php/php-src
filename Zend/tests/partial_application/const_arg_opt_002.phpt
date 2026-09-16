--TEST--
Constant argument optimization - strict_types bug
--CREDITS--
Ryan @ Calif.io
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--FILE--
<?php

declare(strict_types=1);

function cand_86014_typed(int $bound, mixed $placeholder): void
{
    echo 'CALLED:', get_debug_type($bound), ':', $bound, "\n";
}

function cand_86014_make_invalid(): Closure
{
    return cand_86014_typed('123', ?);
}

try {
    $partial = cand_86014_make_invalid();
    echo "CONSTRUCTED\n";
} catch (Throwable $e) {
    echo 'CREATE: ', get_class($e), ': ', $e->getMessage(), "\n";
}

if (isset($partial)) {
    try {
        $partial(null);
    } catch (Throwable $e) {
        echo 'CALL: ', get_class($e), ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
CREATE: TypeError: cand_86014_typed(): Argument #1 ($bound) must be of type int, string given
