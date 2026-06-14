--TEST--
NaN handling: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
function test(float $a) {
    if ($a) var_dump("1");
    if (!$a) var_dump("2");
    var_dump((bool) $a);
    var_dump(!$a);
    echo "\n";
}
function test1(float $a, bool $b) {
    var_dump($a && $b); //JMPNZ_EX
}
function test2(float $a, bool $b) {
    var_dump($a || $b); // JMPZ_EX
}
test(NAN);
test(1.0);
test(0.0);

test1(NAN, true);
test1(1.0, true);
test1(0.0, true);
echo "\n";

test2(NAN, false);
test2(1.0, false);
test2(0.0, false);
?>
--EXPECTF--
Warning: unexpected NAN value was coerced to bool in %s on line 3
string(1) "1"

Warning: unexpected NAN value was coerced to bool in %s on line 4

Warning: unexpected NAN value was coerced to bool in %s on line 5
bool(true)

Warning: unexpected NAN value was coerced to bool in %s on line 6
bool(false)

string(1) "1"
bool(true)
bool(false)

string(1) "2"
bool(false)
bool(true)


Warning: unexpected NAN value was coerced to bool in %s on line 10
bool(true)
bool(true)
bool(false)


Warning: unexpected NAN value was coerced to bool in %s on line 13
bool(true)
bool(true)
bool(false)
