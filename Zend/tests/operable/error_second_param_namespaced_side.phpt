--TEST--
Second parameter of operator method must be global OperandSide, not namespaced OperandSide
--FILE--
<?php

namespace App;

enum OperandSide {
    case Left;
    case Right;
}

readonly class Bad implements \Operable {
    public function __sub(self $o, OperandSide $side): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: The second parameter of operator method App\Bad::__sub() must be OperandSide in %s on line %d
