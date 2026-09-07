--TEST--
GH-20714: Uncatchable exception thrown in generator
--CREDITS--
Grégoire Paris (greg0ire)
--FILE--
<?php

function gen(): Generator {
    try {
        yield 1;
    } finally {}
}

function process(): void {
    $g = gen();
    foreach ($g as $_) {
        throw new Exception('ERROR');
    }
}

try {
    process();
} catch (Exception $e) {
    echo "Caught\n";
}

?>
--EXPECT--
Caught
