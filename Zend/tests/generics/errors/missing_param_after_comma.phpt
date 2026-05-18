--TEST--
Errors: missing parameter after trailing comma plus comma
--FILE--
<?php
try {
    eval('class C<T,,> {}');
} catch (ParseError $e) {
    echo "parse error\n";
}
?>
--EXPECT--
parse error
