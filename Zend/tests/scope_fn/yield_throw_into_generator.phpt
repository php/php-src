--TEST--
Generator::throw into a scope-fn generator: exception propagates through scope_ex body
--FILE--
<?php
function outer() {
    $log = [];
    $fn = fn() {
        try {
            $log[] = "before yield";
            yield 1;
            $log[] = "after yield (unreachable)";
        } catch (RuntimeException $e) {
            $log[] = "caught: " . $e->getMessage();
            yield 99;
        }
    };
    $g = $fn();
    var_dump($g->current());                 // int(1)
    var_dump($g->throw(new RuntimeException("boom"))); // int(99)
    var_dump($log);
}
outer();
echo "done\n";
?>
--EXPECT--
int(1)
int(99)
array(2) {
  [0]=>
  string(12) "before yield"
  [1]=>
  string(12) "caught: boom"
}
done
