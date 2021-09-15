--TEST--
Vector map()
--FILE--
<?php
function dump_vector(Vector $v) {
    printf("count=%d capacity=%d\n", $v->count(), $v->capacity());
    foreach ($v as $i => $value) {
        printf("Value #%d: %s\n", $i, json_encode($value));
    }
}
$create_array = fn($x) => [$x];
dump_vector((new Vector())->map($create_array));
dump_vector((new Vector([strtoupper('test'), true, false, new stdClass(), []]))->map($create_array));
$identity = fn($x) => $x;
dump_vector((new Vector())->map($identity));
dump_vector((new Vector([strtoupper('test'), true, false, new stdClass(), []]))->map($identity));
$false = fn() => false;
dump_vector((new Vector([strtoupper('test'), true, false, new stdClass(), []]))->map($false));
?>
--EXPECT--
count=0 capacity=0
count=5 capacity=5
Value #0: ["TEST"]
Value #1: [true]
Value #2: [false]
Value #3: [{}]
Value #4: [[]]
count=0 capacity=0
count=5 capacity=5
Value #0: "TEST"
Value #1: true
Value #2: false
Value #3: {}
Value #4: []
count=5 capacity=5
Value #0: false
Value #1: false
Value #2: false
Value #3: false
Value #4: false