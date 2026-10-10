--TEST--
IS_IDENTICAL/IS_NOT_IDENTICAL must not use the NOTHROW handler for arrays that may contain arrays
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function identical(array $a, array $b) {
    $r = $a === $b;
    echo "not reached\n";
    return $r;
}

function not_identical(array $a, array $b) {
    $r = $a !== $b;
    echo "not reached\n";
    return $r;
}

$x = [&$x];

try {
    identical($x, [[]]);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), " on line ", $e->getLine(), "\n";
}

try {
    not_identical($x, [[]]);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), " on line ", $e->getLine(), "\n";
}
?>
--EXPECT--
Error: Nesting level too deep - recursive dependency? on line 3
Error: Nesting level too deep - recursive dependency? on line 9
