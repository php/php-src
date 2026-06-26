--TEST--
Errors: missing closing angle in type parameter list
--FILE--
<?php
try {
    eval('class C<T {}');
} catch (ParseError $e) {
    echo "parse error\n";
}
?>
--EXPECT--
parse error
