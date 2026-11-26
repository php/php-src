--TEST--
GH-25756: OPcache "Invalid opcode 62/4/0" when assigning a reference in a return
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

function test_return_ref_assign() {
    $variable_2 = 'success';
    return $variable_1 = &$variable_2;
}

function test_arrow_fn_ref_assign() {
    $variable_2 = 'arrow';
    $fn = fn() => $variable_1 = &$variable_2;
    return $fn();
}

function test_match_ref_assign() {
    $variable_2 = 'match';
    return match (1) {
        1 => $variable_1 = &$variable_2,
    };
}

function test_return_unwraps_ref(&$out) {
    $out = 'before';
    return $local = &$out;
}

echo test_return_ref_assign(), "\n";
echo test_arrow_fn_ref_assign(), "\n";
echo test_match_ref_assign(), "\n";

$ref = null;
$val = test_return_unwraps_ref($ref);
$ref = 'after';
var_dump($val, $ref);

?>
--EXPECT--
success
arrow
match
string(6) "before"
string(5) "after"
