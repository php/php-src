--TEST--
Bug #76074 (opcache corrupts variable in for-loop)
--EXTENSIONS--
opcache
--FILE--
<?php

function test(int $nr) {
    for ($i = $nr; $i <= $nr + 1; $i++)
        var_dump($i);
}

test(1);

?>
--EXPECT--
int(1)
int(2)
