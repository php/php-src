--TEST--
Vector filter()
--FILE--
<?php
function dump_vector(Vector $v) {
    printf("count=%d capacity=%d\n", $v->count(), $v->capacity());
    foreach ($v as $i => $value) {
        printf("Value #%d: %s\n", $i, var_export($value, true));
    }
}
dump_vector((new Vector([false]))->filter());
dump_vector((new Vector([true]))->filter());
dump_vector((new Vector([strtoupper('test'), true, false, new stdClass()]))->filter());
echo "Test functions\n";
$negate = fn($x) => !$x;
dump_vector((new Vector([]))->filter($negate));
dump_vector((new Vector([true]))->filter($negate));
dump_vector((new Vector([strtoupper('test'), true, false, new stdClass(), []]))->filter($negate));
$identity = fn($x) => $x;
dump_vector((new Vector([]))->filter($identity));
dump_vector((new Vector([true]))->filter($identity));
dump_vector((new Vector([strtoupper('test'), true, false, new stdClass(), []]))->filter($identity));
try {
    (new Vector([strtoupper('first'), 'not reached']))->filter(function ($parameter) {
        var_dump($parameter);
        throw new RuntimeException("test");
    });
    echo "Should throw\n";
} catch (Exception $e)  {
    printf("Caught %s: %s\n", get_class($e), $e->getMessage());
}
?>
--EXPECT--
count=0 capacity=0
count=1 capacity=1
Value #0: true
count=3 capacity=3
Value #0: 'TEST'
Value #1: true
Value #2: (object) array(
)
Test functions
count=0 capacity=0
count=0 capacity=0
count=2 capacity=2
Value #0: false
Value #1: array (
)
count=0 capacity=0
count=1 capacity=1
Value #0: true
count=3 capacity=3
Value #0: 'TEST'
Value #1: true
Value #2: (object) array(
)
string(5) "FIRST"
Caught RuntimeException: test
