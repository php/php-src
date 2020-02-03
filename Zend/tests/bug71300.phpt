--TEST--
Bug #71300 (Segfault in zend_fetch_string_offset)
--FILE--
<?php
function test1() {
    for ($n = 'a'; $n < 'g'; $n++) {
        $$n = 1;
    }
    $$n = $$n[++$n] = "test";
    return $$n;
}

var_dump(test1());

function test2() {
    /* See #71303 for why not using for loop here */
    $n = "a";
    $$n .= $$n[++$n] = "test";
    return $$n;
}

var_dump(test2());
?>
--EXPECTF--
string(4) "test"

Warning: Array to string conversion in %s on line %d
string(9) "Arraytest"
