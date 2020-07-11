--TEST--
Valid use of numeric literal separator
--FILE--
<?php
var_dump(299_792_458 === 299792458);
var_dump(135_00 === 13500);
var_dump(96_485.332_12 === 96485.33212);
var_dump(6.626_070_15e-34 === 6.62607015e-34);
var_dump(6.674_083e-11 === 6.674083e-11);
var_dump(0xCAFE_F00D === 0xCAFEF00D);
var_dump(0x54_4A_42 === 0x544A42);
var_dump(0b0101_1111 === 0b01011111);
var_dump(0b01_0000_10 === 0b01000010);
var_dump(0137_041 === 0137041);
var_dump(0_124 === 0124);
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
