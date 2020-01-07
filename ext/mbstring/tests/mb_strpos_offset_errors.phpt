--TEST--
Offset errors for various strpos functions
--FILE--
<?php

try {
    var_dump(mb_strpos("f", "bar", 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strpos("f", "bar", -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strrpos("f", "bar", 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strrpos("f", "bar", -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_stripos("f", "bar", 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_stripos("f", "bar", -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strripos("f", "bar", 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strripos("f", "bar", -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Offset not contained in string
Offset not contained in string
Offset not contained in string
Offset not contained in string
Offset not contained in string
Offset not contained in string
Offset not contained in string
Offset not contained in string
