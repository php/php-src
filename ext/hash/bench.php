#!/usr/bin/env php
<?php

error_reporting(E_ALL);

// 2240 comes from the fact that it used to be file_get_contents(__FILE__);
$data = str_repeat("\x00", 2240);
$time = array();
foreach (hash_algos() as $algo) {
    $time[$algo] = 0;
}

for ($j = 0; $j < 10; $j++) {
    foreach (hash_algos() as $algo) {
        $start = microtime(true);
        for ($i = 0; $i < 1000; $i++) {
            hash($algo, $data);
        }
        $time[$algo] += microtime(true)-$start;
    }
}

asort($time, SORT_NUMERIC);
foreach ($time as $a => $t) {
    printf("%-12s %02.6f\n", $a, $t);
}
