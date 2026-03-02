--TEST--
Additional named params are collect into variadics
--FILE--
<?php

function test($a, string ...$extra) {
    var_dump($a);
    var_dump($extra);
    // Extra named parameters do not contribute toward func_num_args() and func_get_args().
    var_dump(func_num_args());
    var_dump(func_get_args());
}

function test2(&...$refs) {
    foreach ($refs as &$ref) $ref++;
}

test(b: 'b', a: 'a', c: 'c', extra: 'extra');
echo "\n";

test('a', 'b', 'c', d: 'd');
echo "\n";

$x = 0;
$y = 0;
test2(x: $x, y: $y);
var_dump($x, $y);

?>
--EXPECT--
string(1) "a"
array(3) {
  ["b"]=>
  string(1) "b"
  ["c"]=>
  string(1) "c"
  ["extra"]=>
  string(5) "extra"
}
int(1)
array(1) {
  [0]=>
  string(1) "a"
}

string(1) "a"
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "c"
  ["d"]=>
  string(1) "d"
}
int(3)
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}

int(1)
int(1)
