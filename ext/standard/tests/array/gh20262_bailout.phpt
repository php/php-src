--TEST--
GH-20262: Transitive comparison mode is reset on bailout
--FILE--
<?php

function compare($a, $b) {
    var_dump($a < $b);
}

// Test comparison before bailout
compare('5', '3A');

register_shutdown_function(function () {
    // After bailout, transitive mode should be reset
    // This should produce the same result as before
    compare('5', '3A');
});

class Foo {
    public function __toString() {
        eval('class self {}');
        return '';
    }
}

$array = ['5', new Foo];
sort($array);

?>
--EXPECTF--
bool(false)

Fatal error: Cannot use "self" as a class name as it is reserved in %s(%d) : eval()'d code on line %d
bool(false)
