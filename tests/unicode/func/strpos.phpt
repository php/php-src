--TEST--
Unicode: strpos() function test
--INI--
unicode.semantics=on
--FILE--
<?php
$a = "a™ᄒ\U020021z";
var_dump(strpos($a, 'a'));
var_dump(strpos($a, 'U'));
var_dump(strpos($a, 'z'));
var_dump(strpos($a, "\u2122"));
var_dump(strpos($a, 0x1112));
var_dump(strpos($a, 0x20021));

$b = "\U020022z\U020021z";
var_dump(strpos($b, 'z', 1));
var_dump(strpos($b, 'z', 2));
var_dump(strpos($b, 'z', 4));
var_dump(strpos($b, "z\U020021"));

$c = "-A\u030a-Å-Å";
var_dump(strpos($c, 'A'));
var_dump(strpos($c, "\u030a"));
var_dump(strpos($c, "\u00c5"));
var_dump(strpos($c, "\u212b"));

?>
--EXPECT--
int(0)
bool(false)
int(4)
int(1)
int(2)
int(3)
int(1)
int(3)
bool(false)
int(1)
int(1)
int(2)
int(4)
int(6)
