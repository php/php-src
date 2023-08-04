--TEST--
Match expression block must return a value
--FILE--
<?php
function test() {
    var_dump(match (1) {
        1 => {
            echo "Not returning anything\n";
        },
    });
}
try {
    test();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Not returning anything
Match expected a value from block but none was returned
