--TEST--
Forward substring search with binary needles across the compact Sunday table boundary
--FILE--
<?php
$checked = 0;
$failed = 0;
foreach ([9, 15, 16, 30, 31, 32, 33, 254, 255, 256] as $length) {
    // 0xfe occurs only at the end of the planted needle. The other bytes
    // exercise unsigned shift-table indexing, embedded NULs and dense prefixes.
    $needle = substr(str_repeat("\0\x80\xffa", $length), 0, $length - 1) . "\xfe";
    foreach ([1023, 1024, 1025, 8191, 8192, 8193] as $size) {
        $base = substr(str_repeat("\0\x80\xffa", $size), 0, $size);
        foreach ([
            [$base, false, [$base]],
            // Bytes absent from the needle use the default needle_len + 1 shift.
            [str_repeat('z', $size), false, [str_repeat('z', $size)]],
            [$needle . $base, 0, ['', $base]],
            [$base . $needle, $size, [$base, '']],
            // After the first match, the next successful search uses a short remainder.
            [$base . $needle . 'gap' . $needle . 'tail', $size, [$base, 'gap', 'tail']],
        ] as [$haystack, $position, $parts]) {
            $failed += strpos($haystack, $needle) !== $position;
            $failed += str_contains($haystack, $needle) !== ($position !== false);
            $failed += substr_count($haystack, $needle) !== (count($parts) - 1);
            $failed += explode($needle, $haystack) !== $parts;
            $checked++;
        }
    }
}
echo "checked $checked\n";
echo $failed ? "FAILED ($failed)\n" : "ALL OK\n";
?>
--EXPECT--
checked 300
ALL OK
