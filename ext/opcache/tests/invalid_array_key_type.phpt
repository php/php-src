--TEST--
Don't add array value type is key type is illegal
--FILE--
<?php

function test(\SplObjectStorage $definitions = null) {
    $argument = new stdClass;
    $definitions[$argument] = 1;
    $definitions[$argument] += 1;
    $argument = [];
    $definitions[$argument] = 1;
    $definitions[$argument] += 1;
}

?>
===DONE===
--EXPECT--
===DONE===
