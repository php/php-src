--TEST--
reusing anonymous classes
--FILE--
<?php
while (@$i++<10) {
    var_dump(new class{
        public function __construct($i) {
            $this->i = $i;
        }
    }($i));
}
?>
--EXPECTF--	
object(Class$$1)#1 (1) {
  ["i"]=>
  int(1)
}
object(Class$$1)#1 (1) {
  ["i"]=>
  int(2)
}
object(Class$$1)#1 (1) {
  ["i"]=>
  int(3)
}
object(Class$$1)#1 (1) {
  ["i"]=>
  int(4)
}
object(Class$$1)#1 (1) {
  ["i"]=>
  int(5)
}
object(Class$$1)#1 (1) {
  ["i"]=>
  int(6)
}
object(Class$$1)#1 (1) {
  ["i"]=>
  int(7)
}
object(Class$$1)#1 (1) {
  ["i"]=>
  int(8)
}
object(Class$$1)#1 (1) {
  ["i"]=>
  int(9)
}
object(Class$$1)#1 (1) {
  ["i"]=>
  int(10)
}

