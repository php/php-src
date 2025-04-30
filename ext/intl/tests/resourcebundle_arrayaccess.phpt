--TEST--
Test ResourceBundle array access and count - existing/missing keys
--EXTENSIONS--
intl
--FILE--
<?php
    include "resourcebundle.inc";

    // fall back
    $r = new ResourceBundle( 'en_US', BUNDLE );

    printf( "length: %d\n", count($r) );
    printf( "teststring: %s\n", $r['teststring'] );
    printf( "testint: %d\n", $r['testint'] );

    print_r( $r['testvector'] );

    printf( "testbin: %s\n", bin2hex($r['testbin']) );

    $r2 = $r['testtable'];
    printf( "testtable: %d\n", $r2['major'] );

    $r2 = $r['testarray'];
    printf( "testarray: %s\n", $r2[2] );

    echo "Using a reference as an offset:\n";
    $offset = 'teststring';
    $ref = &$offset;
    var_dump($r[$ref]);

    $t = $r['nonexisting'];
    echo debug( $t );
?>
--EXPECT--
length: 6
teststring: Hello World!
testint: 2
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
    [4] => 5
    [5] => 6
    [6] => 7
    [7] => 8
    [8] => 9
    [9] => 0
)
testbin: a1b2c3d4e5f67890
testtable: 3
testarray: string 3
Using a reference as an offset:
string(12) "Hello World!"
NULL
    2: Cannot load resource element 'nonexisting': U_MISSING_RESOURCE_ERROR
