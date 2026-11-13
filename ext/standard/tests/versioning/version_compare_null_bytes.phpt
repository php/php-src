--TEST--
version_compare() compares version strings after NUL bytes
--FILE--
<?php
var_dump(version_compare("1.0\0pl1", '1.0'));
var_dump(version_compare('1.0', "1.0\0pl1"));
var_dump(version_compare("1.0\0pl1", '1.0pl1'));
var_dump(version_compare("1.0\0dev", '1.0', 'lt'));
var_dump(version_compare("\0", ''));
var_dump(version_compare('', "\0"));
var_dump(version_compare("\0", "\0"));
var_dump(version_compare("\x001.0", '.1.0'));
var_dump(version_compare("1.0\0", '1.0'));
var_dump(version_compare("#\0pl1", '#.pl1'));
?>
--EXPECT--
int(1)
int(-1)
int(0)
bool(true)
int(1)
int(-1)
int(0)
int(0)
int(0)
int(0)
