--TEST--
++/-- on structs
--FILE--
<?php

struct Box {
    public function __construct(
        public $value,
    ) {}

    public mutating function preDec() {
        --$this->value;
    }
}

$original = new Box(0);

$preDec = $original;
var_dump(--$preDec->value);

$preInc = $original;
var_dump(++$preInc->value);

$postDec = $original;
var_dump($postDec->value--);

$postInc = $original;
var_dump($postInc->value++);

$preDecMethod = $original;
var_dump($preDecMethod->preDec!());

var_dump($preDec);
var_dump($preInc);
var_dump($postDec);
var_dump($postInc);
var_dump($preDecMethod);
var_dump($original);

?>
--EXPECT--
int(-1)
int(1)
int(0)
int(0)
NULL
object(Box)#2 (1) {
  ["value"]=>
  int(-1)
}
object(Box)#3 (1) {
  ["value"]=>
  int(1)
}
object(Box)#4 (1) {
  ["value"]=>
  int(-1)
}
object(Box)#5 (1) {
  ["value"]=>
  int(1)
}
object(Box)#6 (1) {
  ["value"]=>
  int(-1)
}
object(Box)#1 (1) {
  ["value"]=>
  int(0)
}
