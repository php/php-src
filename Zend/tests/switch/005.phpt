--TEST--
Switch expression discarding result
--FILE--
<?php

switch (1) {
    1 => print "Executed\n",
};

?>
--EXPECT--
Executed
