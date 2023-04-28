<?php

function array_merge_loop($n) {
    $a = range(1, 3);
    for ($i = 0; $i < $n; ++$i) {
        $a = array_merge($a, [1, 2, 3]);
    }
}

function array_merge_loop_with_temporary($n) {
    $a = [];
    for ($i = 0; $i < $n; ++$i) {
        $a = array_merge($a, [1, 2]) + [100];
    }
}

function array_merge_loop_nested($n) {
    $a = [];
    for ($i = 0; $i < $n; ++$i) {
        $a = array_merge($a, [1, 2]) + [$a];
    }
}

function array_unique_loop($n) {
    $a = range(1, 2000);
    $a += $a;
    for ($i = 0; $i < $n; ++$i) {
        $a = array_unique($a);
    }
}

function user_func_helper($array) {
    for ($i = 0; $i < 100; $i++) {
        $array[$i] = $i;
    }
    return $array;
}

function user_func($n) {
    $a = [];
    for ($i = 0; $i < $n; ++$i) {
        $a = user_func_helper($a);
    }
}

/*****/

require 'bench_common.php';

const N = 30000;

$t0 = $t = start_test();
empty_loop(N);
$t = end_test($t, 'empty_loop');
$overhead = $last_time;
array_merge_loop(N);
$t = end_test($t, 'array_merge', $overhead);
array_merge_loop_with_temporary(N);
$t = end_test($t, 'array_merge temp', $overhead);
array_merge_loop_nested(N);
$t = end_test($t, 'array_merge nest', $overhead);
array_unique_loop(N);
$t = end_test($t, 'array_unique', $overhead);
user_func(N);
$t = end_test($t, 'user_func', $overhead);
total($t0, "Total");
