--TEST--
PREG_THROW_ON_ERROR: an array of subjects (or grep inputs) throws on any failing entry, regardless of position
--FILE--
<?php

$bad = "\xff";

$cases = [
    'preg_replace (bad first)'    => fn() => preg_replace('//u', 'x', [$bad, 'ok'], -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace (bad last)'     => fn() => preg_replace('//u', 'x', ['ok', $bad], -1, $c, PREG_THROW_ON_ERROR),
    'preg_filter'                 => fn() => preg_filter('//u', 'x', [$bad, 'ok'], -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace_callback'       => fn() => preg_replace_callback('//u', fn($m) => 'x', [$bad, 'ok'], -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace_callback_array' => fn() => preg_replace_callback_array(['//u' => fn($m) => 'x'], [$bad, 'ok'], -1, $c, PREG_THROW_ON_ERROR),
    'preg_grep (bad first)'       => fn() => preg_grep('//u', [$bad, 'ok'], PREG_THROW_ON_ERROR),
    'preg_grep (bad last)'        => fn() => preg_grep('//u', ['ok', $bad], PREG_THROW_ON_ERROR),
];

foreach ($cases as $name => $case) {
    try {
        $case();
        echo "$name: no exception thrown\n";
    } catch (PregException $e) {
        printf("%s: %s (code matches: %s)\n", $name, $e->getMessage(),
            $e->getCode() === PREG_BAD_UTF8_ERROR ? 'yes' : 'no');
    }
}

?>
--EXPECT--
preg_replace (bad first): Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_replace (bad last): Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_filter: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_replace_callback: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_replace_callback_array: Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_grep (bad first): Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
preg_grep (bad last): Malformed UTF-8 characters, possibly incorrectly encoded (code matches: yes)
