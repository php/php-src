--TEST--
Unicode: strpos() function test
--INI--
unicode.semantics=on
--FILE--
<?php
$a = "a™ᄒ\U020021z";
var_dump(strstr($a, 'a'));
var_dump(strstr($a, 'U'));
var_dump(strstr($a, 'z'));
var_dump(strstr($a, "\u2122"));
var_dump(strstr($a, "\udc21"));
var_dump(strstr($a, 0x1112));
var_dump(strstr($a, 0x20021));

$b = "\U020022z\U020021z";
var_dump(strstr($b, "\U020021"));
var_dump(strstr($b, "z\U020021"));

$c = "-A\u030a-Å-Å";
var_dump(strstr($c, 'A'));
var_dump(strstr($c, "\u030a"));
var_dump(strstr($c, "\u00c5"));
var_dump(strstr($c, "\u212b"));
?>
--EXPECT--
unicode(5) "a™ᄒ𠀡z"
bool(false)
unicode(1) "z"
unicode(4) "™ᄒ𠀡z"
bool(false)
unicode(3) "ᄒ𠀡z"
unicode(2) "𠀡z"
unicode(2) "𠀡z"
unicode(3) "z𠀡z"
unicode(6) "Å-Å-Å"
unicode(5) "̊-Å-Å"
unicode(3) "Å-Å"
unicode(1) "Å"
