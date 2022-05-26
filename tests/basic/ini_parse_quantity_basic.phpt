--TEST--
ini_parse_quantity() function - basic test for ini_parse_quantity()
--FILE--
<?php
foreach (array(
    '-1',
    '0',
    '1',
    '1b',
    '1k',
    '1m',
    '1g',
    '1gb',
    '14.2mb',
    'boat'
) as $input) {
    echo ini_parse_quantity( $input ) . PHP_EOL;
}
?>
--EXPECT--
0
0
1
1
1024
1048576
1073741824
14
0
