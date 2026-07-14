--TEST--
func_get_args() inside scope function including extra args
--FILE--
<?php
function test() {
    $fn = fn($a, $b) {
        return func_get_args();
    };
    // Exact match
    var_dump($fn(10, 20));
    // Extra args beyond declared params
    var_dump($fn(10, 20, 30, 40));
}
test();
?>
--EXPECT--
array(2) {
  [0]=>
  int(10)
  [1]=>
  int(20)
}
array(4) {
  [0]=>
  int(10)
  [1]=>
  int(20)
  [2]=>
  int(30)
  [3]=>
  int(40)
}
