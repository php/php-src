--TEST--
Array variable shorthand does not accept complex variables or function arguments
--FILE--
<?php
foreach (['[:$$name]', '[:$x[0]]', '[:$object->name]', '[:$class::$property]', '[:($name)]', 'foo(:$name)', '[=$name]', '[:&$name]'] as $code) {
    try {
        eval($code . ';');
        echo "accepted\n";
    } catch (ParseError $e) {
        echo "rejected\n";
    }
}
?>
--EXPECT--
rejected
rejected
rejected
rejected
rejected
rejected
rejected
rejected
