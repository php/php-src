--TEST--
NaN handling: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
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
--EXPECT--
string(1) "1"
bool(true)
bool(false)

string(1) "1"
bool(true)
bool(false)

string(1) "2"
bool(false)
bool(true)

bool(true)
bool(true)
bool(false)

bool(true)
bool(true)
bool(false)
