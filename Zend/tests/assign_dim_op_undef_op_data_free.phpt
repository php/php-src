--TEST--
ASSIGN_DIM_OP: error handler releases the array while reporting undefined OP_DATA
--FILE--
<?php
class C { public int $t = 1; }

set_error_handler(function ($_, $msg) {
    echo $msg, "\n";
    if ($msg === 'Undefined variable $undef') {
        $GLOBALS['x'] = 1;
    }
});

$x = [1, 'a'];
$x[1] .= $undef;
var_dump($x);

$x = [1, 2];
$k = 0;
$x[$k] += $undef;
var_dump($x);

$x = [1, 2];
$x[] .= $undef;
var_dump($x);

$x = [[1], 2];
$x[0][0] .= $undef;
var_dump($x);

$x = false;
$x[0] .= $undef;
var_dump($x);

$o = new C;
$x = [1, 2];
$x[0] = &$o->t;
$x[0] += $undef;
var_dump($x, $o->t);
?>
--EXPECT--
Undefined variable $undef
int(1)
Undefined variable $undef
int(1)
Undefined variable $undef
int(1)
Undefined variable $undef
int(1)
Automatic conversion of false to array is deprecated
Undefined array key 0
Undefined variable $undef
int(1)
Undefined variable $undef
int(1)
int(1)
