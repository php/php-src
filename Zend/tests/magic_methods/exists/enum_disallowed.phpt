--TEST--
__exists: disallowed on enums (mirrors __isset)
--FILE--
<?php

enum E {
    public function __exists(string $n): bool { return true; }
}

?>
--EXPECTF--
Fatal error: Enum E cannot include magic method __exists in %s on line %d
