--TEST--
Pass short ctor parameters
--FILE--
<?php

class Empty0 ();

var_dump(new Empty0());

class DTO (
	public int $number,
);

var_dump(new DTO(50));

class Value (int $number);

var_dump(new Value(50));
?>
--EXPECTF--
object(Empty0)#1 (0) {
}
object(DTO)#1 (1) {
  ["number"]=>
  int(50)
}
object(Value)#1 (0) {
}