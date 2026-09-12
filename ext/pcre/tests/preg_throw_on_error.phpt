--TEST--
PREG_THROW_ON_ERROR: every preg_*() function throws PregException on a PCRE error
--FILE--
<?php

$bad = "\xff";

$cases = [
    'preg_match'                   => fn() => preg_match('//u', $bad, $m, PREG_THROW_ON_ERROR),
    'preg_match_all'               => fn() => preg_match_all('//u', $bad, $m, PREG_THROW_ON_ERROR),
    'preg_replace'                 => fn() => preg_replace('//u', 'x', $bad, -1, $c, PREG_THROW_ON_ERROR),
    'preg_filter'                  => fn() => preg_filter('//u', 'x', $bad, -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace_callback'        => fn() => preg_replace_callback('//u', fn($m) => 'x', $bad, -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace_callback_array'  => fn() => preg_replace_callback_array(['//u' => fn($m) => 'x'], $bad, -1, $c, PREG_THROW_ON_ERROR),
    'preg_split'                   => fn() => preg_split('//u', $bad, -1, PREG_THROW_ON_ERROR),
    'preg_grep'                    => fn() => preg_grep('//u', [$bad], PREG_THROW_ON_ERROR),
];

foreach ($cases as $name => $case) {
    try {
        $case();
        echo "$name: no exception thrown\n";
    } catch (PregException $e) {
        printf("%s: %s: %s (code matches: %s)\n", $name, $e::class, $e->getMessage(),
            $e->getCode() === PREG_BAD_UTF8_ERROR ? 'yes' : 'no');
    }
}

?>
--EXPECT--
preg_match: PregException: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_match_all: PregException: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_replace: PregException: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_filter: PregException: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_replace_callback: PregException: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_replace_callback_array: PregException: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_split: PregException: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_grep: PregException: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
