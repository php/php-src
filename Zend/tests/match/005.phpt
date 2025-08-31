--TEST--
Match expression discarding result
--FILE--
<?php

match (1) {
    1 => print "Executed\n",
};

?>
--EXPECT--
Executed
