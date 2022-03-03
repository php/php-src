#!/usr/bin/env php
<?php

error_reporting(E_ALL);

$data = str_repeat("\x00", (int) ($argv[1] ?? (2 * 1024)));
$time = array();
foreach (hash_algos() as $algo) {
    $time[$algo] = 0;
}

for ($j = 0; $j < 10; $j++) {
    foreach (hash_algos() as $algo) {
        $start = hrtime(true);
        for ($i = 0; $i < 1000; $i++) {
            hash($algo, $data);
        }
        $time[$algo] += hrtime(true)-$start;
    }
}

asort($time, SORT_NUMERIC);
foreach ($time as $a => $t) {
    printf("%-12s %02.6f\n", $a, $t/1000000000);
}
