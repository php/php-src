--TEST--
Operator method must declare a parameter type
--FILE--
<?php

readonly class Bad implements Operable {
    public function __pow($x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__pow() must declare a parameter type in %s on line %d
