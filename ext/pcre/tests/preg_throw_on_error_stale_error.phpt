--TEST--
PREG_THROW_ON_ERROR: a replace call that does no matching does not throw a stale error left by an earlier call
--FILE--
<?php

$cases = [
    'preg_replace (empty subject)'          => fn() => preg_replace('/a/', 'x', [], -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace (empty pattern)'          => fn() => preg_replace([], 'x', 'hello', -1, $c, PREG_THROW_ON_ERROR),
    'preg_filter (empty subject)'           => fn() => preg_filter('/a/', 'x', [], -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace_callback (empty subject)' => fn() => preg_replace_callback('/a/', fn($m) => 'x', [], -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace_callback_array (empty)'   => fn() => preg_replace_callback_array(['/a/' => fn($m) => 'x'], [], -1, $c, PREG_THROW_ON_ERROR),
];

foreach ($cases as $label => $case) {
    @preg_match('//u', "\xff");
    $seeded = preg_last_error() === PREG_BAD_UTF8_ERROR;
    try {
        $case();
        printf("%s: seeded=%s, no throw\n", $label, $seeded ? 'yes' : 'no');
    } catch (PregException $e) {
        printf("%s: seeded=%s, THREW %s\n", $label, $seeded ? 'yes' : 'no', $e->getMessage());
    }
}

@preg_match('//u', "\xff");
try {
    preg_replace('//u', 'x', "\xff", -1, $c, PREG_THROW_ON_ERROR);
    echo "genuine error: no throw\n";
} catch (PregException $e) {
    echo "genuine error: threw ", $e->getMessage(), "\n";
}

?>
--EXPECT--
preg_replace (empty subject): seeded=yes, no throw
preg_replace (empty pattern): seeded=yes, no throw
preg_filter (empty subject): seeded=yes, no throw
preg_replace_callback (empty subject): seeded=yes, no throw
preg_replace_callback_array (empty): seeded=yes, no throw
genuine error: threw Malformed UTF-8 characters, possibly incorrectly encoded
