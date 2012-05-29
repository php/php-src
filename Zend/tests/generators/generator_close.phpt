--TEST--
Generator can be closed by calling ->close()
--FILE--
<?php

function *allNumbers() {
    for ($i = 0; true; ++$i) {
        yield $i;
    }
}

$numbers = allNumbers();

foreach ($numbers as $n) {
    var_dump($n);
    if ($n == 9) {
        $numbers->close();
    }
}

?>
--EXPECT--
int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
