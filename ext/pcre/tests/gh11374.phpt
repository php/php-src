--TEST--
GH-11374 PCRE cache entry must not be reused when PCRE JIT flag has changed
--SKIPIF--
<?php
if (ini_get("pcre.jit") === false) {
    die("skip no jit built");
}
--FILE--
<?php

// testing if PCRE cache entry was reused or not is not possible from userland, so instead we test
// if PCRE JIT flag can be cleared and the pcre_match pass

foreach ([true, false, true] as $pcreJit) {
    ini_set('pcre.jit', $pcreJit ? '1' : '0');
    var_dump(ini_get('pcre.jit'));

    var_dump(preg_match('/^a(b+)/', 'abbc', $matches));
    var_dump($matches === ['abb', 'bb']);
}

?>
--EXPECT--
string(1) "1"
int(1)
bool(true)
string(1) "0"
int(1)
bool(true)
string(1) "1"
int(1)
bool(true)
