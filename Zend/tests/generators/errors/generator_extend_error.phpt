--TEST--
The Generator class cannot be extended
--FILE--
<?php

class ExtendedGenerator extends Generator { }

?>
--EXPECTF--
Fatal error: Class ExtendedGenerator may not inherit from final class (Generator) in %s on line %d
