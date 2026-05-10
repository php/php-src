--TEST--
Errors: bare turbofish without function call
--FILE--
<?php
try {
    eval('::<int>;');
} catch (ParseError $e) {
    echo "parse error\n";
}
?>
--EXPECT--
parse error
