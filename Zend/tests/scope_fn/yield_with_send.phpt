--TEST--
Scope-fn generator: send() values into the generator
--FILE--
<?php
function outer() {
    $log = [];
    $fn = fn() {
        $a = yield "first";
        $log[] = "got a=$a";
        $b = yield "second";
        $log[] = "got b=$b";
        return "done";
    };
    $g = $fn();
    var_dump($g->current());
    var_dump($g->send("X"));
    var_dump($g->send("Y"));
    var_dump($g->getReturn());
    var_dump($log);
}
outer();
echo "ok\n";
?>
--EXPECT--
string(5) "first"
string(6) "second"
NULL
string(4) "done"
array(2) {
  [0]=>
  string(7) "got a=X"
  [1]=>
  string(7) "got b=Y"
}
ok
