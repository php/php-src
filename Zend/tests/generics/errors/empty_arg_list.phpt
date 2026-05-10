--TEST--
Errors: empty type argument list
--FILE--
<?php
try {
    eval('function f(array<> $x): void {}');
} catch (ParseError $e) {
    echo "parse error\n";
}
?>
--EXPECT--
parse error
