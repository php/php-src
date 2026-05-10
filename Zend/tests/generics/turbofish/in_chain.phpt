--TEST--
Turbofish: chained calls
--FILE--
<?php
class C {
    public function m<T>() { return $this; }
    public function n<U>($x) { return $x; }
}
$c = new C;
var_dump($c->m::<int>()->n::<string>("hello"));
?>
--EXPECT--
string(5) "hello"
