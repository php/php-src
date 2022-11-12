--TEST--
Collections\Deque offsetGet after push/pop
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
$it = new Collections\Deque();
for ($i = 0; $i < 7; $i++) {
    $it->push("x$i");
}
var_dump($it->popFront());
$it->push('new');
$it->push('another');
for ($i = 0; $i < count($it); $i++) {
    $it[$i] = $it[$i] . "_$i";
    var_dump($it[$i]);
}
foreach ($it as $i => $value) {
    printf("foreach key=%d: %s\n", $i, $value);
}
echo json_encode($it), "\n";
printf("count=%d\n", count($it));

expect_throws(fn() => $it[-1]);
expect_throws(fn() => $it[8]);
--EXPECT--
string(2) "x0"
string(4) "x1_0"
string(4) "x2_1"
string(4) "x3_2"
string(4) "x4_3"
string(4) "x5_4"
string(4) "x6_5"
string(5) "new_6"
string(9) "another_7"
foreach key=0: x1_0
foreach key=1: x2_1
foreach key=2: x3_2
foreach key=3: x4_3
foreach key=4: x5_4
foreach key=5: x6_5
foreach key=6: new_6
foreach key=7: another_7
["x1_0","x2_1","x3_2","x4_3","x5_4","x6_5","new_6","another_7"]
count=8
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
