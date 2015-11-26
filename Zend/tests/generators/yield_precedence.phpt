--TEST--
Precedence of yield and arrow operators
--FILE--
<?php

function gen() {
    yield "a" . "b";
    yield "a" or die;
    yield "k" => "a" . "b";
    yield "k" => "a" or die;
    var_dump([yield "k" => "a" . "b"]);
    yield yield "k1" => yield "k2" => "a" . "b";
    yield yield "k1" => (yield "k2") => "a" . "b";
    var_dump([yield "k1" => yield "k2" => "a" . "b"]);
    var_dump([yield "k1" => (yield "k2") => "a" . "b"]);
}

$g = gen();
for ($g->rewind(), $i = 1; $g->valid(); $g->send($i), $i++) {
    echo "{$g->key()} => {$g->current()}\n";
}

?>
--EXPECT--
0 => ab
1 => a
k => ab
k => a
k => ab
array(1) {
  [0]=>
  int(5)
}
k2 => ab
k1 => 6
2 => 7
3 => k2
k1 => 9
10 => ab
k2 => ab
k1 => 12
array(1) {
  [0]=>
  int(13)
}
11 => k2
k1 => 14
array(1) {
  [15]=>
  string(2) "ab"
}
