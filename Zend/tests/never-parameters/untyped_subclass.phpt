--TEST--
`never` parameter types - on abstract class, subclass has no parameter type
--FILE--
<?php

abstract class BoxedValue {
    abstract public function get(): mixed;
    abstract public function set(never $v): void;
}

class BoxedAnything extends BoxedValue {
    public function __construct(
        private mixed $value
    ) {}

    public function get(): mixed {
        return $this->value;
    }
    public function set($v): void {
        $this->value = $v;
    }
}

$box = new BoxedAnything(5);
var_dump($box);
var_dump($box->get());
$box->set("testing");
var_dump($box);
var_dump($box->get());

?>
--EXPECTF--
object(BoxedAnything)#%d (1) {
  ["value":"BoxedAnything":private]=>
  int(5)
}
int(5)
object(BoxedAnything)#%d (1) {
  ["value":"BoxedAnything":private]=>
  string(7) "testing"
}
string(7) "testing"
