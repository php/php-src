#!/usr/bin/env php
/* This is a generated file, do not modify */
/* Usage: php create_data_file.php /path/to/magic.mgc > data_file.c */
<?php
    /*--- Initialization of our translation table ---*/

    for ($i = 0; $i < 0x100; ++$i) {
        $map[chr($i)] = sprintf('\x%02X', $i);
    }
    // \0 is a shortcut for \x00; as the majority of the input file is \0's,
    // we divide the generated file's size by nearly 2 (30 MB -> 16 MB).
    $map[chr(0)] = '\0';

    /*--- File generation ---*/

    // https://github.com/php/php-src/pull/10422
    // Some compilers (GCC, clang) do not like long lists; some (MSVC) do not like long strings.
    // CHUNK_SIZE splitting our ~10 MB binary source should give a good compromise between both.
    const CHUNK_SIZE = 1024;

    $dta = file_get_contents( $argv[1] );
    $chunks = str_split($dta, CHUNK_SIZE);
    $chunks[count($chunks) - 1] = str_pad($chunks[count($chunks) - 1], CHUNK_SIZE, chr(0));

    echo 'const unsigned char php_magic_database[' . count($chunks) . '][' . CHUNK_SIZE . "] = {\n";
    foreach ($chunks as $chunk) {
        echo '"' . strtr($chunk, $map) . "\",\n";
    }
    echo "};\n";
?>
