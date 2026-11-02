--TEST--
strtotime() rejects strings containing NUL bytes
--CREDITS--
HackingRepo (relunsec@insitetech.jp)
--FILE--
<?php

var_dump(strtotime("\0"));
var_dump(strtotime("2026-11-02\0"));
var_dump(strtotime("2026-11-02\0 tomorrow"));
var_dump(strtotime("tomorrow\0"));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
