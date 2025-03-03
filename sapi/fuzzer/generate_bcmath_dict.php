<?php

if (!extension_loaded('bcmath')) {
    echo "Skipping bcmath dictionary generation\n";
    return;
}

define('BCMATH_GEN_DICT_LOOP', 300);

function genRandomNumberString(): string
{
    return match (rand(0, 2)) {
        0 => (string) rand(0, 1000000),
        1 => rand(0, 1000000) . rand(0, 1000000),
        2 => rand(0, 1000000) . rand(0, 1000000) . rand(0, 1000000),
    };
}

function genRandomNumber(bool $integerIsZero, bool $hasFraction): string
{
    $sign = rand(0, 1) ? '' : '-';
    $integer = $integerIsZero ? '0' : genRandomNumberString();
    $fraction = $hasFraction ? '.' . genRandomNumberString() : '';
    return $sign . $integer . $fraction;
}

function writeDict(array $dicts): void
{
    $dict = implode("\n", $dicts) . "\n";
    file_put_contents(__DIR__ . "/dict/bcmath", $dict, FILE_APPEND);
}

$total = 0;

for ($loop = 0; $loop < BCMATH_GEN_DICT_LOOP; $loop++) {
    $case1 = [true, true];
    $case2 = [false, true];
    $case3 = [false, false];

    foreach ([
        [
            $case1,
            $case1,
        ],
        [
            $case1,
            $case2,
        ],
        [
            $case1,
            $case3,
        ],
        [
            $case2,
            $case1,
        ],
        [
            $case2,
            $case2,
        ],
        [
            $case2,
            $case3,
        ],
        [
            $case3,
            $case1,
        ],
        [
            $case3,
            $case2,
        ],
        [
            $case3,
            $case3,
        ],
    ] as [
        [$integerIsZero, $hasFraction],
        [$integerIsZero2, $hasFraction2],
    ]) {
        //$dicts = [];
        for ($i = 0; $i < 100; $i++) {
            $dividend = genRandomNumber($integerIsZero, $hasFraction);
            $divisor = genRandomNumber($integerIsZero2, $hasFraction2);
            $scale = rand(0, 100);
            //$dicts[] = '"' . implode(',', [$dividend, $divisor, $scale]) . '"';
            $dict = '"' . implode(',', [$dividend, $divisor, $scale]) . '"' . "\n";
            file_put_contents(__DIR__ . "/corpus/bcmath/{$total}", $dict);
            $total++;
        }
        //writeDict($dicts);
    }
}
