--TEST--
Structs must not be marked as final
--FILE--
<?php

final struct Box {}

?>
--EXPECTF--
Fatal error: Cannot use the final modifier on a struct, as structs are implicitly final in %s on line %d
