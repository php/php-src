#!/usr/bin/env php
<?php

if ($argc < 2) {
    echo "Usage: php gen_rare_cp_bitvec.php ./common_codepoints.txt\n";
    return;
}

$bitvec = array_fill(0, (0xFFFF / 32) + 1, 0xFFFFFFFF);

$input = file_get_contents($argv[1]);
foreach (explode("\n", $input) as $line) {
    if (false !== $hashPos = strpos($line, '#')) {
        $line = substr($line, 0, $hashPos);
    }

    $line = trim($line);
    if ($line === '') {
        continue;
    }

    $range = explode("\t", $line);
    $start = hexdec($range[0]);
    $end   = hexdec($range[1]);

    for ($i = $start; $i <= $end; $i++) {
        $bitvec[$i >> 5] &= ~(1 << ($i & 0x1F));
    }
}

$result = <<<'HEADER'
/* Machine-generated file; do not edit! See gen_rare_cp_bitvec.php.
 *
 * The below array has one bit for each Unicode codepoint from U+0000 to U+FFFF.
 * The bit is 1 if the codepoint is considered 'rare' for the purpose of
 * guessing the text encoding of a string.
 *
 * Each 'rare' codepoint which appears in a string when it is interpreted
 * using a candidate encoding causes the candidate encoding to be treated
 * as less likely to be the correct one.
 */

static const uint32_t rare_codepoint_bitvec[] = {
HEADER;

for ($i = 0; $i < 0xFFFF / 32; $i++) {
    if ($i % 8 === 0) {
        $result .= "\n";
    } else {
        $result .= " ";
    }

    $result .= "0x" . str_pad(dechex($bitvec[$i]), 8, '0', STR_PAD_LEFT) . ",";
}

$result .= "\n};\n";

file_put_contents(__DIR__ . '/rare_cp_bitvec.h', $result);

echo "Done.\n";
?>
