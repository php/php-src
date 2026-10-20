--TEST--
mb_convert_variables(): $vars which are not string|array|object
--EXTENSIONS--
mbstring
--FILE--
<?php

$i = 5;
$a = [$i];

try {
    var_dump( mb_convert_variables('UTF-8', 'EUC-JP', $i, $a));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump( mb_convert_variables('UTF-8', 'EUC-JP', $a));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Warning: mb_convert_variables(): Argument #3 must be of type string|array|object or only contain entries of type string|array|object, int given in %s on line %d

Warning: mb_convert_variables(): Argument #4 must be of type string|array|object or only contain entries of type string|array|object, int given in %s on line %d
string(6) "EUC-JP"

Warning: mb_convert_variables(): Argument #3 must be of type string|array|object or only contain entries of type string|array|object, int given in %s on line %d
string(6) "EUC-JP"
