--TEST--
Incorrect optimization of $i = $i++
--EXTENSIONS--
opcache
--FILE--
<?php

function test() {
    $i = 1;
    $i = $i++;
    var_dump($i);

    $i = 1;
    $i = $i--;
    var_dump($i);
}
test();

?>
--EXPECT--
int(1)
int(1)
