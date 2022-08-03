--TEST--
GH-8074 (Wrong type inference of range() result)
--FILE--
<?php
function test() {
    $array = range(1, "2");

    foreach ($array as $i) {
        var_dump($i + 1);
    }
}

test();
?>
--EXPECT--
int(2)
int(3)
