--TEST--
GH-11942 (IntlDateFormatter should canonicalize locale strings)
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

echo "Testing IntlDateFormatter locale canonicalization:\n";
foreach ($test_cases as $test_case) {
    [$input, $expected] = $test_case;
    
    $formatter = new IntlDateFormatter($input, IntlDateFormatter::SHORT, IntlDateFormatter::NONE, 'UTC');
    $actual = $formatter->getLocale();

    $status = ($actual === $expected) ? 'PASS' : 'FAIL';
    echo "Input: $input -> Expected: $expected -> Actual: $actual -> $status\n";
}

$dateFormatter = new IntlDateFormatter('pt_PT.utf8', IntlDateFormatter::SHORT, IntlDateFormatter::NONE, 'UTC');
$dateResult = $dateFormatter->format(1691585260);
echo "\nDateFormatter with pt_PT.utf8: " . $dateResult . "\n";
?>
--EXPECT--
Testing IntlDateFormatter locale canonicalization:
Input: pt -> Expected: pt -> Actual: pt -> PASS
Input: pt-PT -> Expected: pt_PT -> Actual: pt_PT -> PASS
Input: pt_PT.utf8 -> Expected: pt_PT -> Actual: pt_PT -> PASS
Input: fr_CA@euro -> Expected: fr_CA -> Actual: fr_CA -> PASS

DateFormatter with pt_PT.utf8: 09/08/23
