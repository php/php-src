--TEST--
Block Pass 003: Inorrect constant substitution in FETCH_LIST_R
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function test() {
    for ($i = 0; $i < 10; $i++) {
        list($a, $b) = 1 ? 1 : 2;
    }
}
test();
?>
DONE
--EXPECT--
DONE
