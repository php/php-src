--TEST--
Can use readonly as a function name
--FILE--
<?php

function readonly() {
    echo "Hi!\n";
}

readonly();
readonly ();

?>
--EXPECT--
Hi!
Hi!
