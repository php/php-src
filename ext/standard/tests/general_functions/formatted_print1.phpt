--TEST--
println() function with strict_types=0
--FILE--
<?php

// Normally, you would never bother checking the number of bytes returned,
// but this is a unit test of the function itself.
$v1 = println('');
$v2 = println('println always adds a newline');
println("the above string was $v2 bytes including the newline, the below appends \\n to \\n");
$v3 = println("\n");
$v4 = println('strict_types=0 allows string coercion the exact same way as any other function');
$v5 = println(1);
$v6 = println(true);
$v7 = println();  // the default argument is the empty string
println('println byte lengths');
println(json_encode([$v1, $v2, $v3, $v4, $v5, $v6, $v7]));
try {
    println([]);
} catch (TypeError $e) {
    echo "Caught TypeError {$e->getMessage()}\n";
}
try {
    println('test', 'too many');
} catch (ArgumentCountError $e) {
    echo "Caught ArgumentCountError {$e->getMessage()}\n";
}

?>
--EXPECT--
println always adds a newline
the above string was 30 bytes including the newline, the below appends \n to \n


strict_types=0 allows string coercion the exact same way as any other function
1
1

println byte lengths
[1,30,2,79,2,2,1]
Caught TypeError println(): Argument #1 ($data) must be of type string, array given
Caught ArgumentCountError println() expects at most 1 argument, 2 given