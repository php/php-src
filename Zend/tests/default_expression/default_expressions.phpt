--TEST--
Tests an exhaustive list of valid expressions containing the default keyword
--FILE--
<?php

function F($V = 2) { return $V; }

var_dump(F(default + 1));
var_dump(F(default - 1));
var_dump(F(default * 2));
var_dump(F(default / 2));
var_dump(F(default % 2));
var_dump(F(default & 1));
var_dump(F(default | 1));
var_dump(F(default ^ 2));
var_dump(F(default << 1));
var_dump(F(default >> 1));
var_dump(F(default ** 2));
echo PHP_EOL;
var_dump(F(1 + default));
var_dump(F(1 - default));
var_dump(F(2 * default));
var_dump(F(2 / default));
var_dump(F(2 % default));
var_dump(F(1 & default));
var_dump(F(1 | default));
var_dump(F(2 ^ default));
var_dump(F(1 << default));
var_dump(F(1 >> default));
var_dump(F(2 ** default));
?>
--EXPECT--
int(3)
int(1)
int(4)
int(1)
int(0)
int(0)
int(3)
int(0)
int(4)
int(1)
int(4)

int(3)
int(-1)
int(4)
int(1)
int(0)
int(0)
int(3)
int(0)
int(4)
int(0)
int(4)
