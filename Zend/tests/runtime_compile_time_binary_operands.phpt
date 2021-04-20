--TEST--
Test binary operands exposing the same behavior at compile as at run time
--INI--
memory_limit=256M
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
    "+",
    "-",
    "*",
    "/",
    "%",
    "**",
    ".",
    "|",
    "&",
    "^",
    "or",
    "and",
    "xor",
    "||",
    "&&",
];
$unaryOperators = [
    "~",
    "-",
    "+",
    "!",
];

$input = [
    0,
    1,
    2,
    -1,
    2.0,
    2.1,
    -2.0,
    -2.1,
    PHP_INT_MAX,
    PHP_INT_MIN,
    PHP_INT_MAX * 2,
    PHP_INT_MIN * 2,
    INF,
    NAN,
    [],
    [1, 2],
    [1, 2, 3],
    [1 => 2, 0 => 1],
    [1, 'a' => 2],
    [1, 4],
    [1, 'a'],
    [1, 2 => 2],
    [1, [ 2 ]],
    null,
    false,
    true,
    "",
    " ",
    "banana",
    "Banana",
    "banan",
    "0",
    "200",
    "20",
    "20a",
    " \t\n\r\v\f20",
    "20  ",
    "2e1",
    "2e150",
    "9179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368",
    "-9179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368",
    "0.1",
    "-0.1",
    "1e-1",
    "-20",
    "-20.0",
    "0x14",
    (string) PHP_INT_MAX * 2,
    (string) PHP_INT_MIN * 2,
];

function makeParam($param) {
    if ($param === PHP_INT_MIN) {
        return "PHP_INT_MIN";
    }
    if ($param === PHP_INT_MAX) {
        return "PHP_INT_MAX";
    }
    if (is_string($param)) {
        return '"' . strtr($param, ["\t" => '\t', "\n" => '\n', "\r" => '\r', "\v" => '\v', "\f" => '\f', '$' => '\$', '"' => '\"']) . '"';
    }
    return "(" . str_replace("\n", "", var_export($param, true)) . ")";
}

$c = 0;
$f = 0;

function prepareBinaryLine($op1, $op2, $cmp, $operator) {
    $op1_p = makeParam($op1);
    $op2_p = makeParam($op2);

    $error = "echo '" . addcslashes("$op1_p $operator $op2_p", "\\'") . '\', "\n"; $f++;';

    $compare = "@($op1_p $operator $op2_p)";
    $line = "\$c++; ";
    try {
        $result = makeParam($cmp());
        $line .= "if (" . ($result === "(NAN)" ? "!is_nan($compare)" : "$compare !== $result") . ") { $error }";
    } catch (Error $e) {
        $msg = makeParam($e->getMessage());
        $line .= "try { $compare; $error } catch (Error \$e) { if (\$e->getMessage() !== $msg) { $error } }";
    }
    return $line;
}
function prepareUnaryLine($op, $cmp, $operator) {
    $op_p = makeParam($op);

    $error = "echo '" . addcslashes("$operator $op_p", "\\'") . '\', "\n"; $f++;';

    $compare = "@($operator $op_p)";
    $line = "\$c++; ";
    try {
        $result = makeParam($cmp());
        $line .= "if (" . ($result === "(NAN)" ? "!is_nan($compare)" : "$compare !== $result") . ") { $error }";
    } catch (Error $e) {
        $msg = makeParam($e->getMessage());
        $line .= "try { $compare; $error } catch (Error \$e) { if (\$e->getMessage() !== $msg) { $error } }";
    }
    return $line;
}

$filename = __DIR__ . DIRECTORY_SEPARATOR . 'compare_binary_operands_temp.php';
$file = fopen($filename, "w");

fwrite($file, "<?php\n");

foreach ($input as $left) {
    foreach ($input as $right) {
        foreach ($binaryOperators as $operator) {
            $line = prepareBinaryLine($left, $right, function() use ($left, $right, $operator) {
                return eval("return @(\$left $operator \$right);");
            }, $operator);
            fwrite($file, $line . "\n");
        }
    }
}
foreach ($input as $right) {
    foreach ($unaryOperators as $operator) {
        $line = prepareUnaryLine($right, function() use ($right, $operator) {
            return eval("return @($operator \$right);");
        }, $operator);
        fwrite($file, $line . "\n");
    }
}

fclose($file);

include $filename;

if($c === 0) {
    echo "Completely failed\n";
} else {
    echo "Failed: $f\n";
}
?>
--CLEAN--
<?php
$fl = __DIR__ . DIRECTORY_SEPARATOR . 'compare_binary_operands_temp.php';
@unlink($fl);
?>
--EXPECT--
Failed: 0
