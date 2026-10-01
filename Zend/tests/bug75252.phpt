--TEST--
Bug #75252: Incorrect token formatting on two parse errors in one request
--FILE--
<?php

$code = <<<'CODE'
function test_missing_semicolon() : string {
     $x = []
     FOO
}
CODE;

try {
    eval($code);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    eval($code);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ParseError: syntax error, unexpected identifier "FOO"
ParseError: syntax error, unexpected identifier "FOO"
