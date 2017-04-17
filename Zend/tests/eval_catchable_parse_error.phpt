--TEST--
eval() throws catchable parse error
--FILE--
<?php

try {
    eval("substr('foo', 1");
} catch (ParseError $ex) {
    var_dump(1);
}

?>
--EXPECT--
int(1)
