<?php
return function (array &$r, array &$ram, int $budget): int {
    $r4 = $r[4]; $r5 = $r[5]; $r14 = $r[14]; $r15 = $r[15]; $r24 = $r[24]; $r25 = $r[25]; $r29 = $r[29]; $r31 = $r[31];
    $flag = $r[0];
    $p = 0x108;
    $r14 = $ram[$p >> 2];
    $r29 = ($r29 - 24) & 0xFFFFFFFF;
    $a = $r29 + 20;
    $ram[$a >> 2] = $r31;          // a different slot
    $a = $r14;
    if (($a & 0x7F800003) === 0) {
    }
    $r4 = $r14;
    if ($budget === 0) {
        return 1;
    }
    if (($a & 0x7F800003) === 0) {
    } else {
        if (!$flag) {
        }
    }
    $a = $r29 + 20;
    if (($a & 0x7F800003) === 0) {
        $r31 = $ram[$p >> 2];          // should read slot 0x108 >> 2 again
    }
    $r[4] = $r4; $r[5] = $r5; $r[14] = $r14; $r[15] = $r15; $r[24] = $r24; $r[25] = $r25; $r[29] = $r29; $r[31] = $r31;
    return $r31;
};
