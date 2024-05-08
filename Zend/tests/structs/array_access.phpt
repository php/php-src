--TEST--
Structs implementing ArrayAccess
--FILE--
<?php

abstract class VectorBase implements ArrayAccess {
    public $elements;

    public function offsetExists(mixed $offset): bool {
        return array_key_exists($offset, $this->elements);
    }

    public function offsetSet(mixed $offset, mixed $value): void {
        if ($offset) {
            $this->elements[$offset] = $value;
        } else {
            $this->elements[] = $value;
        }
    }

    public function offsetUnset(mixed $offset): void {
        unset($this->elements[$offset]);
    }
}

struct VectorByVal extends VectorBase {
    public function offsetGet(mixed $offset): mixed {
        return $this->elements[$offset];
    }
}

struct VectorByRef extends VectorBase {
    public function &offsetGet(mixed $offset): mixed {
        return $this->elements[$offset];
    }
}

struct Box {
    public function __construct(
        public int $value,
    ) {}

    public mutating function inc() {
        $this->value++;
    }
}

$box = new Box(1);

$vec = new VectorByVal();
$vec[] = $box;
$vec[0]->value = 2;
var_dump($vec);
$vec[0]->inc!();
var_dump($vec);

$vec = new VectorByRef();
$vec[] = $box;
$vec[0]->value = 2;
var_dump($vec);
$vec[0]->inc!();
var_dump($vec);

var_dump($box);

?>
--EXPECT--
object(VectorByVal)#2 (1) {
  ["elements"]=>
  array(1) {
    [0]=>
    object(Box)#1 (1) {
      ["value"]=>
      int(1)
    }
  }
}
object(VectorByVal)#2 (1) {
  ["elements"]=>
  array(1) {
    [0]=>
    object(Box)#1 (1) {
      ["value"]=>
      int(1)
    }
  }
}
object(VectorByRef)#3 (1) {
  ["elements"]=>
  array(1) {
    [0]=>
    object(Box)#2 (1) {
      ["value"]=>
      int(2)
    }
  }
}
object(VectorByRef)#3 (1) {
  ["elements"]=>
  array(1) {
    [0]=>
    object(Box)#2 (1) {
      ["value"]=>
      int(3)
    }
  }
}
object(Box)#1 (1) {
  ["value"]=>
  int(1)
}
