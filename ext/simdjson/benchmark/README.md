# Benchmark

## Install PHP Composer dependencies

[Install Composer](https://getcomposer.org/download/) if not already done and execute it in the benchmark folder:

```
composer install
```

## Run PHPBench benchmark

Execute the following command from the `ext/simdjson` folder:

```
./../../sapi/cli/php ./benchmark/vendor/bin/phpbench run --report=table --group decode
```

The output should look like this:

```
\SimdjsonBench\DecodeBench

    jsonDecodeAssoc.........................R4 I4 [μ Mo]/r: 0.00972 0.00978 (ms) [μSD μRSD]/r: 0.000ms 2.10%
    jsonDecode..............................R1 I2 [μ Mo]/r: 0.01060 0.01060 (ms) [μSD μRSD]/r: 0.000ms 1.19%
    simdjsonDecodeAssoc.....................R5 I4 [μ Mo]/r: 0.00580 0.00580 (ms) [μSD μRSD]/r: 0.000ms 3.08%
    simdjsonDecode..........................R5 I4 [μ Mo]/r: 0.00680 0.00680 (ms) [μSD μRSD]/r: 0.000ms 1.86%

4 subjects, 20 iterations, 20 revs, 0 rejects, 0 failures, 0 warnings
(best [mean mode] worst) = 5.600 [8.230 8.246] 6.000 (μs)
⅀T: 164.600μs μSD/r 0.159μs μRSD/r: 2.059%
suite: 1343d64965925446273f4815a25108531738fcaf, date: 2020-08-04, stime: 16:30:36
+-------------+---------------------+--------+----------+-----------+-----------+-------+
| benchmark   | subject             | groups | mem_peak | mean      | best      | diff  |
+-------------+---------------------+--------+----------+-----------+-----------+-------+
| DecodeBench | simdjsonDecodeAssoc | decode | 880,864b | 0.00580ms | 0.00560ms | 1.00x |
| DecodeBench | simdjsonDecode      | decode | 880,856b | 0.00680ms | 0.00660ms | 1.17x |
| DecodeBench | jsonDecodeAssoc     | decode | 880,856b | 0.00972ms | 0.00940ms | 1.68x |
| DecodeBench | jsonDecode          | decode | 880,856b | 0.01060ms | 0.01040ms | 1.83x |
+-------------+---------------------+--------+----------+-----------+-----------+-------+
```

```
./../../sapi/cli/php ./benchmark/vendor/bin/phpbench run --report=table --group key_value
```

The output should look like this:

```
\SimdjsonBench\KeyValueBench

    jsonDecode..............................R1 I4 [μ Mo]/r: 0.00968 0.00962 (ms) [μSD μRSD]/r: 0.000ms 2.11%
    simdjsonDeepString......................R1 I2 [μ Mo]/r: 0.00240 0.00240 (ms) [μSD μRSD]/r: 0.000ms 0.00%
    simdjsonDeepStringAssoc.................R1 I2 [μ Mo]/r: 0.00240 0.00240 (ms) [μSD μRSD]/r: 0.000ms 0.00%
    simdjsonInt.............................R3 I4 [μ Mo]/r: 0.00200 0.00200 (ms) [μSD μRSD]/r: 0.000ms 0.00%
    simdjsonIntAssoc........................R2 I1 [μ Mo]/r: 0.00220 0.00220 (ms) [μSD μRSD]/r: 0.000ms 0.00%
    simdjsonArray...........................R1 I2 [μ Mo]/r: 0.00340 0.00340 (ms) [μSD μRSD]/r: 0.000ms 0.00%
    simdjsonObject..........................R1 I2 [μ Mo]/r: 0.00400 0.00400 (ms) [μSD μRSD]/r: 0.000ms 0.00%

7 subjects, 35 iterations, 35 revs, 0 rejects, 0 failures, 0 warnings
(best [mean mode] worst) = 2.000 [3.726 3.717] 2.000 (μs)
⅀T: 130.400μs μSD/r 0.029μs μRSD/r: 0.301%
suite: 1343d642240d34ab476549affaa92a81d4f7ce57, date: 2020-08-04, stime: 16:31:43
+---------------+-------------------------+-----------+----------+-----------+-----------+-------+
| benchmark     | subject                 | groups    | mem_peak | mean      | best      | diff  |
+---------------+-------------------------+-----------+----------+-----------+-----------+-------+
| KeyValueBench | simdjsonInt             | key_value | 884,496b | 0.00200ms | 0.00200ms | 1.00x |
| KeyValueBench | simdjsonIntAssoc        | key_value | 884,504b | 0.00220ms | 0.00220ms | 1.10x |
| KeyValueBench | simdjsonDeepString      | key_value | 884,504b | 0.00240ms | 0.00240ms | 1.20x |
| KeyValueBench | simdjsonDeepStringAssoc | key_value | 884,504b | 0.00240ms | 0.00240ms | 1.20x |
| KeyValueBench | simdjsonArray           | key_value | 884,496b | 0.00340ms | 0.00340ms | 1.70x |
| KeyValueBench | simdjsonObject          | key_value | 884,496b | 0.00400ms | 0.00400ms | 2.00x |
| KeyValueBench | jsonDecode              | key_value | 884,496b | 0.00968ms | 0.00940ms | 4.84x |
+---------------+-------------------------+-----------+----------+-----------+-----------+-------+
```

```
./../../sapi/cli/php ./benchmark/vendor/bin/phpbench run --report=table --group multiple
```

The output should look like this:

```
\SimdjsonBench\MultipleAccessBench

    simdjsonMultipleAccessSameDocument......R1 I4 [μ Mo]/r: 0.01500 0.01481 (ms) [μSD μRSD]/r: 0.000ms 2.39%
    simdjsonMultipleAccessDifferentDocument.R5 I4 [μ Mo]/r: 0.01512 0.01537 (ms) [μSD μRSD]/r: 0.000ms 2.31%

2 subjects, 10 iterations, 10 revs, 0 rejects, 0 failures, 0 warnings
(best [mean mode] worst) = 14.600 [15.060 15.088] 15.400 (μs)
⅀T: 150.600μs μSD/r 0.353μs μRSD/r: 2.346%
suite: 1343d643ab682962f20a6f8c0f5615c1b987a7bc, date: 2020-08-04, stime: 16:29:21
+---------------------+-----------------------------------------+----------+----------+-----------+-----------+-------+
| benchmark           | subject                                 | groups   | mem_peak | mean      | best      | diff  |
+---------------------+-----------------------------------------+----------+----------+-----------+-----------+-------+
| MultipleAccessBench | simdjsonMultipleAccessSameDocument      | multiple | 903,496b | 0.01500ms | 0.01460ms | 1.00x |
| MultipleAccessBench | simdjsonMultipleAccessDifferentDocument | multiple | 903,496b | 0.01512ms | 0.01460ms | 1.01x |
+---------------------+-----------------------------------------+----------+----------+-----------+-----------+-------+
```

## Run benchmark

You may also run the custom benchmarks by executing the following command:

```
./../../sapi/cli/php ./benchmark/benchmark.php
```
