<?php

spl_autoload_register(function($class) {
    if ($class == 'Bar') {
        class Bar {
            const BAZ = 42;

            public self $x;
            public Foo $y;
        }
    } else if ($class == 'Foo') {
        class Foo {}
    }
});

class Test {
    const FOO = Bar::BAZ;
}
