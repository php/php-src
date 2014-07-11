--TEST--
Generator shouldn't crash if last yielded value is a closure
--FILE--
<?php

function gen() {
    yield function() {};
}

$gen = gen();
$gen->next();

echo "Done!";

?>
--EXPECT--
Done!
