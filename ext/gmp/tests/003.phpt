--TEST--
Check for number base recognition
--EXTENSIONS--
gmp
--FILE--
<?php
        /* Binary */
        $test[] = gmp_init("0b10011010010");
        $test[] = gmp_init("0b10011010010", 2);
        $test[] = gmp_init("10011010010");
        $test[] = gmp_init("10011010010", 2);

        /* Octal */
        $test[] = gmp_init("02322");
        $test[] = gmp_init("02322", 8);
        $test[] = gmp_init("2322");
        $test[] = gmp_init("2322", 8);

        /* Decimal */
        $test[] = gmp_init("1234");
        $test[] = gmp_init("1234", 10);

        /* Hexadecimal */
        $test[] = gmp_init("0x4d2");
        $test[] = gmp_init("0x4d2", 16);
        try {
            $test[] = gmp_init("4d2");
        } catch (\ValueError $e) {
            echo $e->getMessage() . \PHP_EOL;
        }
        $test[] = gmp_init("4d2", 16);

        for ($i = 0; $i < count($test); $i++) {
                printf("%s\n", gmp_strval($test[$i]));
        }
?>
--EXPECT--
gmp_init(): Argument #1 ($num) is not an integer string
1234
1234
10011010010
1234
1234
1234
2322
1234
1234
1234
1234
1234
1234
