--TEST--
mb_convert_case() with UTF-8 strings that start with ASCII characters
--EXTENSIONS--
mbstring
--FILE--
<?php
/* UTF-8 case conversion handles a leading run of ASCII characters separately
 * and hands over to the general code at a multiple of 64 bytes. Check
 * strings whose first non-ASCII character falls on either side of that
 * boundary. */
mb_substitute_character(0x3F);
$ascii = "The Quick Brown Fox Jumps Over the Lazy Dog 0123456789 '.:^`[]{}@~";
for ($len = 0; $len <= 200; $len++) {
    $prefix = substr(str_repeat($ascii, 4), 0, $len);
    $upper = strtoupper($prefix);
    $lower = strtolower($prefix);
    $checks = [
        [MB_CASE_UPPER, $prefix . "éß\xFFz", $upper . "ÉSS?Z"],
        [MB_CASE_UPPER_SIMPLE, $prefix . "éß\xFFz", $upper . "Éß?Z"],
        [MB_CASE_LOWER, $prefix . "ÉİZ", $lower . "éi̇z"],
        [MB_CASE_LOWER_SIMPLE, $prefix . "ÉİZ", $lower . "éiz"],
        [MB_CASE_FOLD, $prefix . "ÉßZ", $lower . "éssz"],
        [MB_CASE_FOLD_SIMPLE, $prefix . "ÉßZ", $lower . "éßz"],
        /* Final sigma depends on the character before it */
        [MB_CASE_LOWER, $prefix . "a''Σ", $lower . "a''ς"],
        [MB_CASE_LOWER, $prefix . " ''Σ", $lower . " ''σ"],
        [MB_CASE_LOWER, $prefix . "a''ΣA", $lower . "a''σa"],
    ];
    foreach ($checks as [$mode, $in, $expected]) {
        $out = mb_convert_case($in, $mode, 'UTF-8');
        if ($out !== $expected) {
            echo "Mode $mode, length $len: got ", bin2hex($out), ", expected ", bin2hex($expected), "\n";
        }
    }
    if (mb_strtoupper($prefix, 'UTF-8') !== $upper || mb_strtolower($prefix, 'UTF-8') !== $lower) {
        echo "ASCII string of length $len\n";
    }
}

/* The sigma check looks back up to 64 characters into the ASCII prefix */
var_dump(mb_strtolower(str_repeat("A", 64) . "Σ", 'UTF-8') === str_repeat("a", 64) . "ς");
var_dump(mb_strtolower(str_repeat("A", 60) . str_repeat(".", 4) . "Σ", 'UTF-8') === str_repeat("a", 60) . "....ς");
var_dump(mb_strtolower(str_repeat(" ", 127) . "Σ", 'UTF-8') === str_repeat(" ", 127) . "σ");
echo "Done\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
Done
