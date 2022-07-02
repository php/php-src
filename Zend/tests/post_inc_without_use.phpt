--TEST--
POST_INC without use during DFA optimization
--FILE--
<?php

function test($n) {
    for ($i = 0; $i < $n; !$i++) {}
}

?>
===DONE===
--EXPECT--
===DONE===
