--TEST--
Test block expression pretty printing
--FILE--
<?php

assert((function () {
    return {
        echo 'Foo' . PHP_EOL;
        {
            echo 'Bar' . PHP_EOL;
        }
        false
    };
})());

assert(10 + {
    echo 'Foo';
    -10
});

assert({ false });

?>
--EXPECTF--
Foo
Bar

Warning: assert(): assert(function () {
    return {
        echo 'Foo' . PHP_EOL;
        {
            echo 'Bar' . PHP_EOL;
        }
        false
    };
}()) failed in %s on line %d
Foo
Warning: assert(): assert(10 + {
    echo 'Foo';
    -10
}) failed in %s on line %d

Warning: assert(): assert({
    false
}) failed in %s on line %d
