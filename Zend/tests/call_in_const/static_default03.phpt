--TEST--
Can call internal functions from defaults of static variables
--FILE--
<?php
function main() {
    static $call = missing_sprintf("%s!", sprintf("Hello, %s", "World"));
    echo "$call\n";
}
for ($i = 0; $i < 2; $i++) {
    try {
        main();
    } catch (Error $e) {
        printf("Caught %s: %s at line %d\n", get_class($e), $e->getMessage(), $e->getLine());
    }
}
?>
--EXPECT--
Caught Error: Call to undefined function missing_sprintf() at line 3
Caught Error: Call to undefined function missing_sprintf() at line 3
