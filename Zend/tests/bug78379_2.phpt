--TEST--
Bug #78379.2 (Cast to object confuses GC, causes crash)
--FILE--
<?php
class E {}
function f() {
    $e1 = new E;
    $e2 = new E;
    $a = ['e2' => $e2];
    $e1->a = (object)$a;
    $e2->e1 = $e1;
    $e2->a = (object)$a;
}
f();
gc_collect_cycles();
echo "End\n";
?>
--EXPECT--
End
