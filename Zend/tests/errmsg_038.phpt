--TEST--
errmsg: properties cannot be final
--FILE--
<?php

class test {
    final $var = 1;
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use the final modifier on a property in %s on line %d
