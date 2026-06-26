--TEST--
Cross-stack scope fn: multiple suspend / resume cycles before parent exits
--FILE--
<?php
function outer() {
    $log = [];
    $fn = fn() {
        $log[] = "a";
        Fiber::suspend("s1");
        $log[] = "b";
        Fiber::suspend("s2");
        $log[] = "c";
    };
    $fiber = new Fiber($fn);
    var_dump($fiber->start());  // s1
    var_dump($fiber->resume()); // s2
    var_dump($fiber->resume()); // NULL (fiber returned)
    var_dump($log);
}
try {
    outer();
    echo "no error\n";
} catch (Error $e) {
    echo "unexpected: ", $e->getMessage(), "\n";
}
echo "done\n";
?>
--EXPECT--
string(2) "s1"
string(2) "s2"
NULL
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
no error
done
