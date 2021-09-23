--TEST--
Vector setSize with $default
--FILE--
<?php
function show(Vector $vec) {
    printf("value=%s count=%d\n", json_encode($vec), $vec->count());
}

$vec = new Vector();
show($vec);
$vec->setSize(2, 0);
$vec[0] = new stdClass();
$vec->setSize(3, null);
show($vec);
$vec->setSize(0, new stdClass());
show($vec);
$vec->setSize(5, strtoupper('na'));
show($vec);
?>
--EXPECT--
value=[] count=0
value=[{},0,null] count=3
value=[] count=0
value=["NA","NA","NA","NA","NA"] count=5
