--TEST--
No additional parenthesis are required around yield if they are already present
--FILE--
<?php

function gen() {
    if (yield $foo); elseif (yield $foo);
    if (yield $foo): elseif (yield $foo): endif;
    while (yield $foo);
    do {} while (yield $foo);
    switch (yield $foo) {}
    (yield $foo);
    die(yield $foo);
    func(yield $foo);
    $foo->func(yield $foo);
    new Foo(yield $foo);
}

echo "Done";

?>
--EXPECT--
Done
