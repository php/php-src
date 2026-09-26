--TEST--
metaphone() $maxPhonemes overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
try {
    metaphone('hello', PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

error_reporting(E_ALL & ~E_DEPRECATED);

/* metaphone() allocates $max_phonemes + 1 bytes with zend_string_alloc(),
 * which adds the header and the terminating NUL to that without an overflow
 * check. Its own limit is therefore one below PHP_STRING_MAX_LENGTH, so the
 * window of refused values starts at the constant itself rather than above
 * it. Beyond the limit the allocation wraps to a tiny buffer whose ZSTR_LEN
 * is huge and the phoneme loop writes past the end of it; a long word is used
 * so that an unfixed build overruns far enough to fault instead of corrupting
 * the heap silently. */
$sizeMax = 2 ** (PHP_SYS_SIZE * 8) - 1;
$word = str_repeat('Thompson', 1000);

$lengths = [
    'STR_MAX'    => PHP_STRING_MAX_LENGTH,
    'STR_MAX+1'  => PHP_STRING_MAX_LENGTH + 1,
    'STR_MAX+2'  => PHP_STRING_MAX_LENGTH + 2,
    'SIZE_MAX-2' => $sizeMax - 2,
    'SIZE_MAX-1' => $sizeMax - 1,
    'SIZE_MAX'   => $sizeMax,
];

foreach ($lengths as $label => $length) {
    try {
        metaphone($word, $length);
        echo "$label: unexpected success\n";
    } catch (ValueError $e) {
        echo "$label: {$e->getMessage()}\n";
    }
}
?>
--EXPECTF--
Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
metaphone(): Argument #2 ($max_phonemes) must be less than or equal to %d
STR_MAX: metaphone(): Argument #2 ($max_phonemes) must be less than or equal to %d
STR_MAX+1: metaphone(): Argument #2 ($max_phonemes) must be less than or equal to %d
STR_MAX+2: metaphone(): Argument #2 ($max_phonemes) must be less than or equal to %d
SIZE_MAX-2: metaphone(): Argument #2 ($max_phonemes) must be less than or equal to %d
SIZE_MAX-1: metaphone(): Argument #2 ($max_phonemes) must be less than or equal to %d
SIZE_MAX: metaphone(): Argument #2 ($max_phonemes) must be less than or equal to %d
