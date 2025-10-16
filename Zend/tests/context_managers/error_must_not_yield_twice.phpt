--TEST--
Context manager: manager must not yield twice
--FILE--
<?php

function invalid_manager() {
    yield new stdClass;
    yield new stdClass;
}

try {
    using (invalid_manager() => $var) {
        echo "Body executed\n";
    };
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
Body executed
Error: Context Manager must yield only once, but it yielded a second time when fetching the return value
