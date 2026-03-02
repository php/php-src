--TEST--
GH-11942 (NumberFormatter should canonicalize locale strings)
--EXTENSIONS--
intl
--FILE--
<?php

$test_cases = [
    ['pt', 'pt'],
    ['pt-PT', 'pt_PT'],
    ['pt_PT.utf8', 'pt_PT'],
    ['fr_CA@euro', 'fr_CA'],
];

echo "Testing NumberFormatter locale canonicalization:\n";
foreach ($test_cases as $test_case) {
    [$input, $expected] = $test_case;
    
    $formatter = new NumberFormatter($input, NumberFormatter::DECIMAL);
    $actual = $formatter->getLocale();

    $status = ($actual === $expected) ? 'PASS' : 'FAIL';
    echo "Input: $input -> Expected: $expected -> Actual: $actual -> $status\n";
}

$numFormatter = new NumberFormatter('pt_PT.utf8', NumberFormatter::DECIMAL);
$numResult = $numFormatter->format(1234.56);
echo "\nNumberFormatter with pt_PT.utf8: " . $numResult . "\n";
?>
--EXPECT--
Testing NumberFormatter locale canonicalization:
Input: pt -> Expected: pt -> Actual: pt -> PASS
Input: pt-PT -> Expected: pt_PT -> Actual: pt_PT -> PASS
Input: pt_PT.utf8 -> Expected: pt_PT -> Actual: pt_PT -> PASS
Input: fr_CA@euro -> Expected: fr_CA -> Actual: fr_CA -> PASS

NumberFormatter with pt_PT.utf8: 1 234,56
