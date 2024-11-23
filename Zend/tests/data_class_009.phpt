--TEST--
unset triggers copy-on-write
--FILE--
<?php

$data = new data class(1) {
    public function __construct(public int $value) {}
};

$copy = $data;
unset($copy->value);
var_dump($data !== $copy);
var_dump($data);
?>
--EXPECT--
bool(true)
data object(class@anonymous)#1 (1) {
  ["value"]=>
  int(1)
}
