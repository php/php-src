--TEST--
Deferred destructor: a WeakReference to the dying object returns null immediately
--FILE--
<?php
class C {
    public function __destruct() {
        echo "dtor\n";
    }
}

$o = new C;
$wr = WeakReference::create($o);
unset($o);
$v = $wr->get();
echo $v === null ? "null\n" : "live\n";
echo "end\n";
?>
--EXPECT--
null
end
dtor
