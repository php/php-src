--TEST--
IO hooks: Io\Operation cannot be extended in userland
--FILE--
<?php
class MyOperation extends Io\Operation {}
?>
--EXPECTF--
Fatal error: Class MyOperation cannot extend final class Io\Operation in %s on line %d
