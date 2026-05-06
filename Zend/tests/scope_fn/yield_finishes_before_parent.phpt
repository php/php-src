--TEST--
Scope-fn generator finishes before parent exits: no escape error
--FILE--
<?php
function outer() {
    $log = [];
    $fn = fn() {
        $log[] = "a";
        yield 1;
        $log[] = "b";
        yield 2;
        $log[] = "c";
    };
    $g = $fn();
    foreach ($g as $v) {
        echo "got: $v\n";
    }
    var_dump($log);
    $g = null; /* drop ref before scope fn closure ref */
}
outer();
echo "done\n";
?>
--EXPECT--
got: 1
got: 2
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
done
