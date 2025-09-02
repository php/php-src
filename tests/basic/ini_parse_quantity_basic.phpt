--TEST--
ini_parse_quantity() function - basic test for ini_parse_quantity()
--INI--
error_reporting = E_ALL ^ E_WARNING
--FILE--
<?php
foreach (array(
    '-1',
    '-0x412',
    '0',
    '1',
    '1b',
    '1k',
    '1m',
    '1g',
    '1gb',
    '14.2mb',
    '14.2bm',
    'boat'
) as $input) {
    echo ini_parse_quantity( $input ) . PHP_EOL;
}
?>
--EXPECT--
-1
-1042
0
1
1
1024
1048576
1073741824
1
14
14680064
0
