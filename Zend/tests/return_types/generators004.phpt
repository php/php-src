--TEST--
Generator with return type does not fail with empty return
--FILE--
<?php

$a = function(): \Iterator {
    yield 1;
    return;
};

foreach($a() as $value) {
    echo $value;
}
?>
--EXPECT--
1
