--TEST--
Errors: empty type parameter list
--FILE--
<?php
try {
    eval('class Box<> {}');
} catch (ParseError $e) {
    echo "parse error\n";
}
?>
--EXPECT--
parse error
