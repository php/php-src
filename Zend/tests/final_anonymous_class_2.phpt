--TEST--
Final anonymous classes
--FILE--
<?php

$x = new final class {};
class_alias($x::class, 'alias');
class aliasExtends extends alias {}

?>
--EXPECTF--
Fatal error: Class aliasExtends cannot extend final class class@anonymous in %s on line %d
