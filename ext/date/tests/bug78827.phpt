--TEST--
Bug #78827 (Wrong timestamp)
--FILE--
<?php
var_dump((strtotime('last monday + 7 days') - strtotime('last monday'))/86400);
var_dump((strtotime('last tuesday + 7 days') - strtotime('last tuesday'))/86400);
var_dump((strtotime('last wednesday + 7 days') - strtotime('last wednesday'))/86400);
var_dump((strtotime('last thursday + 7 days') - strtotime('last thursday'))/86400);
var_dump((strtotime('last friday + 7 days') - strtotime('last friday'))/86400);
var_dump((strtotime('last saturday + 7 days') - strtotime('last saturday'))/86400);
var_dump((strtotime('last sunday + 7 days') - strtotime('last sunday'))/86400);
?>
--EXPECT--
int(7)
int(7)
int(7)
int(7)
int(7)
int(7)
int(7)
