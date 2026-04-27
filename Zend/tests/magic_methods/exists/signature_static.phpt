--TEST--
__exists: must not be static
--FILE--
<?php

class StaticExists {
    public static function __exists(string $n): bool { return true; }
}
?>
--EXPECTF--
Fatal error: Method StaticExists::__exists() cannot be static in %s on line %d
