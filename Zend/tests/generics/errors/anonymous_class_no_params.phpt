--TEST--
Errors: anonymous classes cannot declare type parameters
--FILE--
<?php
try {
    eval('$x = new class<T> {};');
} catch (ParseError $e) {
    echo "parse error: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
parse error: %s
