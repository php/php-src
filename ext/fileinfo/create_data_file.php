#!/usr/bin/env php
/* This is a generated file, do not modify */
/* Usage: php create_data_file.php /path/to/magic.mgc > data_file.c */
<?php
    /*--- Initialization of our translation table ---*/

    // By default, everything gets mapped to its \o notation
    // (not \x, because by C's norm, \x eats as many chars as possible, while \o stops at exactly 3;
    // thus \x0ABACK_2_MALICE is interpreted as hex \x0ABAC (which overflows) followed by string K_2_MALICE,
    // while \o0120 is unambiguously a CR followed by digit 0).
    for ($i = 0; $i < 0x100; ++$i) {
        $map[chr($i)] = sprintf('\%03o', $i);
    }
    // \0 is a shortcut for \x00; as the majority of the input file is \0's,
    // we divide the generated file's size by nearly 2 (30 MB -> 16 MB).
    $map[chr(0)] = '\0';
    $map["\n"] = '\n';
    // Displayable ASCII can be output as is: strings for file types will appear readable.
    for ($i = ord(' '); $i < 0x7F; ++$i) {
        $map[chr($i)] = chr($i);
    }
    // … Except digits following a \0: \012 will be interpreted as octal 012, and not \0 followed by 12.
    // Then we have to express \0 in a full unambiguous 3-chars octal code.
    for ($i = ord('0'); $i <= ord('9'); ++$i) {
        $map[chr(0).chr($i)] = '\000'.chr($i);
    }
    // … Except " and \ because we enclose the result into quotes and escape with \.
    $map['"'] = '\"';
    $map['\\'] = '\\\\';

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
        echo '"' . strtr($chunk, $map) . '",' . "\n";
    }
    echo "};\n";
?>
