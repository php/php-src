--TEST--
ASSIGN_DIM: error handler releases the array while reporting undefined OP_DATA
--FILE--
<?php
set_error_handler(function ($_, $msg) {
    echo $msg, "\n";
    if ($msg === 'Undefined variable $undef') {
        $GLOBALS['x'] = 1;
    }
});

$x = [1, 2];
$x[0] = $undef;
var_dump($x);

$x = [1, 2];
$x['k'] = $undef;
var_dump($x);

$x = [1, 2];
$k = 5;
$x[$k] = $undef;
var_dump($x);

$x = [1, 2];
$x[] = $undef;
var_dump($x);

$x = [[1], 2];
$x[0][1] = $undef;
var_dump($x);

$x = false;
$x[0] = $undef;
var_dump($x);

$x = [1, 2];
$r = ($x[0] = $undef);
var_dump($x, $r);
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
Undefined variable $undef
int(1)
Automatic conversion of false to array is deprecated
Undefined variable $undef
int(1)
Undefined variable $undef
int(1)
NULL
