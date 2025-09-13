--TEST--
NAN coerced to other types
--FILE--
<?php

$binaryOperators = [
    "==",
    "!=",
    "===",
    "!==",
    "<",
    "<=",
    ">",
    ">=",
    "<=>",
];
$inputs = [
    0,
    null,
    false,
    true,
    "",
    [],
    NAN,
];

$nan = fdiv(0, 0);
var_dump($nan);
foreach ($inputs as $right) {
    echo 'Using ';
    var_export($right);
    echo ' as right op', PHP_EOL;
    var_dump($nan == $right);
    var_dump($nan != $right);
    var_dump($nan === $right);
    var_dump($nan !== $right);
    var_dump($nan < $right);
    var_dump($nan <= $right);
    var_dump($nan > $right);
    var_dump($nan >= $right);
    var_dump($nan <=> $right);
}

?>
--CLEAN--
<?php
$fl = __DIR__ . DIRECTORY_SEPARATOR . 'compare_binary_operands_nan_temp.php';
@unlink($fl);
?>
--EXPECT--
float(NAN)
Using 0 as right op
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
int(1)
Using NULL as right op
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
int(1)
Using false as right op
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
int(1)
Using true as right op
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
int(1)
Using '' as right op
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
int(1)
Using array (
) as right op
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
int(1)
Using NAN as right op
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
int(1)

