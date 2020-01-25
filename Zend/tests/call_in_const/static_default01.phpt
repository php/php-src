--TEST--
Can call internal functions from defaults of static variables
--FILE--
<?php
function main() {
    static $call = SPRINTF("%s!", sprintf("Hello, %s", "World"));
    echo "$call\n";
}
main();
main();
?>
--EXPECT--
Hello, World!
Hello, World!
