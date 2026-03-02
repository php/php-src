--TEST--
implementing the same interface twice
--FILE--
<?php

interface foo {
}

class bar implements foo, foo {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Class bar cannot implement previously implemented interface foo in %s on line %d
