--TEST--
Switch expression discarding result
--FILE--
<?php

1 switch {
    1 => print "Executed\n",
};

?>
--EXPECT--
Executed
