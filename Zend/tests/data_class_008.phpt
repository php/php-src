--TEST--
Data classes can be anonymous
--FILE--
<?php

$data = new data class(1) {
    public function __construct(private int $value) {}
};

var_dump($data);
?>
--EXPECT--
data object(class@anonymous)#1 (1) {
  ["value":"class@anonymous":private]=>
  int(1)
}
