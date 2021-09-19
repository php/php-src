--TEST--
Collections\Deque push/pop/unshift/shift
--FILE--
<?php

function expect_throws(Closure $cb): void {
    try {
        $cb();
        echo "Unexpectedly didn't throw\n";
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

function dump_it(Collections\Deque $dq) {
    printf("count=%d %s\n", $dq->count(), json_encode($dq));
}

$it = new Collections\Deque([]);
dump_it($it);
expect_throws(fn() => $it->pop());
expect_throws(fn() => $it->shift());
$it->unshift(strtolower('HELLO'));
dump_it($it);
$it->push(strtolower('WORLD'));
dump_it($it);
foreach ($it as $key => $value) {
    printf("%s: %s\n", json_encode($key), json_encode($value));
}
for ($i = 0; $i < 50; $i++) {
    $it->push("y$i");
    $it->unshift("x$i");
}
dump_it($it);

?>
--EXPECT--
count=0 []
Caught UnderflowException: Cannot pop from empty Collections\Deque
Caught UnderflowException: Cannot shift from empty Collections\Deque
count=1 ["hello"]
count=2 ["hello","world"]
0: "hello"
1: "world"
count=102 ["x49","x48","x47","x46","x45","x44","x43","x42","x41","x40","x39","x38","x37","x36","x35","x34","x33","x32","x31","x30","x29","x28","x27","x26","x25","x24","x23","x22","x21","x20","x19","x18","x17","x16","x15","x14","x13","x12","x11","x10","x9","x8","x7","x6","x5","x4","x3","x2","x1","x0","hello","world","y0","y1","y2","y3","y4","y5","y6","y7","y8","y9","y10","y11","y12","y13","y14","y15","y16","y17","y18","y19","y20","y21","y22","y23","y24","y25","y26","y27","y28","y29","y30","y31","y32","y33","y34","y35","y36","y37","y38","y39","y40","y41","y42","y43","y44","y45","y46","y47","y48","y49"]