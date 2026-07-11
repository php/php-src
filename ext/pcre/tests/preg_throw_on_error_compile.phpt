--TEST--
PREG_THROW_ON_ERROR: a malformed pattern throws PregException carrying the same error as preg_last_error()/preg_last_error_msg(), with no warning
--FILE--
<?php

$bad = '/[/';

$cases = [
    'preg_match'                  => fn() => preg_match($bad, 'x', $m, PREG_THROW_ON_ERROR),
    'preg_match_all'              => fn() => preg_match_all($bad, 'x', $m, PREG_THROW_ON_ERROR),
    'preg_replace'                => fn() => preg_replace($bad, 'r', 'x', -1, $c, PREG_THROW_ON_ERROR),
    'preg_filter'                 => fn() => preg_filter($bad, 'r', 'x', -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace_callback'       => fn() => preg_replace_callback($bad, fn($m) => 'r', 'x', -1, $c, PREG_THROW_ON_ERROR),
    'preg_replace_callback_array' => fn() => preg_replace_callback_array([$bad => fn($m) => 'r'], 'x', -1, $c, PREG_THROW_ON_ERROR),
    'preg_split'                  => fn() => preg_split($bad, 'x', -1, PREG_THROW_ON_ERROR),
    'preg_grep'                   => fn() => preg_grep($bad, ['x'], PREG_THROW_ON_ERROR),
];

foreach ($cases as $name => $case) {
    try {
        $case();
        echo "$name: no exception thrown\n";
    } catch (PregException $e) {
        printf("%s: %s | matches preg_last_error()/msg(): %s\n",
            $name, $e->getMessage(),
            ($e->getCode() === preg_last_error() && $e->getMessage() === preg_last_error_msg()) ? 'yes' : 'no');
    }
}

echo "\n";

$sites = [
    'empty pattern'       => '',
    'bad delimiter'       => '1a1',
    'no ending delimiter' => '/x',
    'unknown modifier'    => '/x/Z',
    'unterminated class'  => '/[/',
];
foreach ($sites as $label => $bad) {
    try {
        preg_match($bad, 'x', $m, PREG_THROW_ON_ERROR);
        echo "$label: no exception thrown\n";
    } catch (PregException $e) {
        printf("%s: %s | matches: %s\n", $label, $e->getMessage(),
            ($e->getCode() === preg_last_error() && $e->getMessage() === preg_last_error_msg()) ? 'yes' : 'no');
    }
}

?>
--EXPECT--
preg_match: Internal error | matches preg_last_error()/msg(): yes
preg_match_all: Internal error | matches preg_last_error()/msg(): yes
preg_replace: Internal error | matches preg_last_error()/msg(): yes
preg_filter: Internal error | matches preg_last_error()/msg(): yes
preg_replace_callback: Internal error | matches preg_last_error()/msg(): yes
preg_replace_callback_array: Internal error | matches preg_last_error()/msg(): yes
preg_split: Internal error | matches preg_last_error()/msg(): yes
preg_grep: Internal error | matches preg_last_error()/msg(): yes

empty pattern: Internal error | matches: yes
bad delimiter: Internal error | matches: yes
no ending delimiter: Internal error | matches: yes
unknown modifier: Internal error | matches: yes
unterminated class: Internal error | matches: yes
