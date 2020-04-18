#!/usr/bin/env php
/* This is a generated file, do not modify */
/* Usage: php create_data_file.php /path/to/magic.mgc > data_file.c */
<?php
    $dta = file_get_contents( $argv[1] );
    $dta_l = strlen($dta);
    $j = 0;

    echo "const unsigned char php_magic_database[$dta_l] = {\n";
    for ($i = 0; $i < $dta_l; $i++) {
        printf("0x%02X, ", ord($dta[$i]));
        if ($j % 16 == 15) {
            echo "\n";
        }
        $j++;
    }
    echo "};\n";
?>
