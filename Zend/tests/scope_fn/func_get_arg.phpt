--TEST--
func_get_arg() inside scope function including extra args
--FILE--
<?php
function test() {
    $fn = fn($a) {
        $results = [];
        for ($i = 0; $i < func_num_args(); $i++) {
            $results[] = func_get_arg($i);
        }
        return $results;
    };
    var_dump($fn(10, 20, 30));
}
test();
?>
--EXPECT--
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(20)
  [2]=>
  int(30)
}
