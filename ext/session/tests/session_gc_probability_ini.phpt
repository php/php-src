--TEST--
Test session.gc_probability and session.gc_divisor settings for invalid values
--INI--
session.gc_maxlifetime=1
--EXTENSIONS--
session
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php

$gc_settings = [
    [
        'gc_probability' => -1,
        'gc_divisor' => -1
    ],
    [
        'gc_probability' => -1,
        'gc_divisor' => 1
    ],
    [
        'gc_probability' => 1,
        'gc_divisor' => -1
    ],
    [
        'gc_probability' => 1,
        'gc_divisor' => 0
    ],
];

ob_start();
foreach($gc_settings as $gc_setting) {
try {
    session_start($gc_setting);
    session_write_close();
    } catch (Throwable $e) {
        echo $e::class, ': '. $e->getMessage(), "\n";
    }
}
ob_end_flush();
?>
Done
--EXPECTF--
Warning: session_start(): session.gc_probability must be greater than or equal to 0 in %s on line %d

Warning: session_start(): Setting option "gc_probability" failed in %s on line %d

Warning: session_start(): session.gc_divisor must be greater than 0 in %s on line %d

Warning: session_start(): Setting option "gc_divisor" failed in %s on line %d

Warning: session_start(): session.gc_probability must be greater than or equal to 0 in %s on line %d

Warning: session_start(): Setting option "gc_probability" failed in %s on line %d

Warning: session_start(): session.gc_divisor must be greater than 0 in %s on line %d

Warning: session_start(): Setting option "gc_divisor" failed in %s on line %d

Warning: session_start(): session.gc_divisor must be greater than 0 in %s on line %d

Warning: session_start(): Setting option "gc_divisor" failed in %s on line %d
Done
