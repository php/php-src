--TEST--
Test session.gc_probability and session.gc_divisor settings for negative values - always run GC
--INI--
session.gc_maxlifetime=1
--EXTENSIONS--
session
--SKIPIF--
<?php
include('skipif.inc');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
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
];

ob_start();
foreach($gc_settings as $gc_setting) {
    session_start($gc_setting);
    $_SESSION['test'] = 'test';
    session_write_close();

    sleep(2);
    session_start($gc_setting);
    if(count($_SESSION) === 0) {
        echo "\nERROR";
    }
    session_write_close();
    $_SESSION = [];

    session_start($gc_setting);
    if(count($_SESSION) !== 0) {
        echo "\nERROR";
    }
    session_destroy();
}
ob_end_flush();
?>
Done
--EXPECTF--
Done
