--TEST--
yield @throw new Exception
--FILE--
<?php

function generator() {
    yield 1;
    yield @throw new Exception();
    yield 2;
}

foreach (generator() as $val) {
    var_dump($val);
}

echo "Done\n";
?>
--EXPECT--
int(1)
NULL
int(2)
Done
