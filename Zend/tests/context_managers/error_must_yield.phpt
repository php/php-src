--TEST--
Context manager: manager must yield once
--FILE--
<?php

function invalid_manager() {
    if (false) {
        yield;
    }
}

try {
    using (invalid_manager() => $var) {
        echo "Not executed\n";
    };
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Context Manager must yield once, but it returned without yielding
