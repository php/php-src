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
} catch (ParseError $e) {
    var_dump($e->getMessage());
}

try {
    eval($code);
} catch (ParseError $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(41) "syntax error, unexpected identifier "FOO""
string(41) "syntax error, unexpected identifier "FOO""
