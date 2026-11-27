--TEST--
Non-readonly class cannot implement Operable
--FILE--
<?php

class Bad implements Operable {
    public function __add(int $x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Class Bad implements Operable and must be declared readonly in %s on line %d
