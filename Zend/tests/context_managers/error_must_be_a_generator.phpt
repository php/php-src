--TEST--
Context manager: manager must be a Generator
--FILE--
<?php

try {
    using (new stdClass) {
        echo "Not executed\n";
    };
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Context Manager must be an instance of Generator (TODO: proper error message)
