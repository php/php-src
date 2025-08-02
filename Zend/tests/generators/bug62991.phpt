--TEST--
Bug #62991 (Segfault with generator and closure)
--FILE--
<?php

function test( array $array )
{
    $closure = function() use ( $array ) {
        print_r( $array );
        yield "hi";
    };
    return $closure();
}

function test2( array $array )
{
    $closure = function() use ( $array ) {
        print_r( $array );
        yield "hi";
    };
    return $closure; // if you return the $closure and call it outside this function it works.
}

$generator = test(array( 1, 2, 3 ) );
foreach($generator as $something) {
}

$generator = test2(array( 1, 2, 3 ) );
foreach($generator() as $something) {
}


$generator = test2(array( 1, 2, 3 ) );

echo "okey\n";
?>
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
okey
