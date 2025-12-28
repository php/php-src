--TEST--
GH-16886 (ini_parse_quantity() fails to emit warning for 0x+0)
--FILE--
<?php
echo ini_parse_quantity('0x 0'), "\n";
echo ini_parse_quantity('0x+0'), "\n";
echo ini_parse_quantity('0x-0'), "\n";
echo ini_parse_quantity('0b 0'), "\n";
echo ini_parse_quantity('0b+0'), "\n";
echo ini_parse_quantity('0b-0'), "\n";
echo ini_parse_quantity('0o 0'), "\n";
echo ini_parse_quantity('0o+0'), "\n";
echo ini_parse_quantity('0o-0'), "\n";
?>
--EXPECTF--
Warning: Invalid quantity "0x 0": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
0

Warning: Invalid quantity "0x+0": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
0

Warning: Invalid quantity "0x-0": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
0

Warning: Invalid quantity "0b 0": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
0

Warning: Invalid quantity "0b+0": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
0

Warning: Invalid quantity "0b-0": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
0

Warning: Invalid quantity "0o 0": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
0

Warning: Invalid quantity "0o+0": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
0

Warning: Invalid quantity "0o-0": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
0
