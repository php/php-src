--TEST--
`never` parameter types - basic usage with an interface
--FILE--
<?php

interface BoxedValue {
    public function get(): mixed;
    public function set(never $v): void;
}

class BoxedInt implements BoxedValue {
    public function __construct(
        private int $value
    ) {}

    public function get(): int {
        return $this->value;
    }
    public function set(int $v): void {
        $this->value = $v;
    }
}

$box = new BoxedInt(5);
var_dump($box);
var_dump($box->get());
$box->set(7);
var_dump($box);
var_dump($box->get());

?>
--EXPECTF--
object(BoxedInt)#%d (1) {
  ["value":"BoxedInt":private]=>
  int(5)
}
int(5)
object(BoxedInt)#%d (1) {
  ["value":"BoxedInt":private]=>
  int(7)
}
int(7)
