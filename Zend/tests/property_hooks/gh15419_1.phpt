--TEST--
GH-15419: Readonly classes may not declare properties with hooks
--FILE--
<?php

readonly class C {
    public int $prop { set => $value; }
}

?>
--EXPECTF--
Fatal error: Hooked properties cannot be readonly in %s on line %d
