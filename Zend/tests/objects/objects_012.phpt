--TEST--
implementing a class
--FILE--
<?php

class foo {
}

interface bar extends foo {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: bar cannot implement foo - it is not an interface in %s on line %d
