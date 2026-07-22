--TEST--
PREG_THROW_ON_ERROR: a malformed pattern still warns exactly as without the flag, and additionally throws PregException carrying the generic error preg_last_error()/preg_last_error_msg() report
--FILE--
<?php

$warned = false;
set_error_handler(function ($errno, $errstr) use (&$warned) {
    $warned = true;
    return true;
});

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
    $warned = false;
    try {
        $case();
        echo "$name: no exception thrown\n";
    } catch (PregException $e) {
        printf("%s: %s | warned: %s | matches: %s\n",
            $name, $e->getMessage(),
            $warned ? 'yes' : 'no',
            ($e->getCode() === preg_last_error() && $e->getMessage() === preg_last_error_msg()) ? 'yes' : 'no');
    }
}

restore_error_handler();

?>
--EXPECT--
preg_match: Internal error | warned: yes | matches: yes
preg_match_all: Internal error | warned: yes | matches: yes
preg_replace: Internal error | warned: yes | matches: yes
preg_filter: Internal error | warned: yes | matches: yes
preg_replace_callback: Internal error | warned: yes | matches: yes
preg_replace_callback_array: Internal error | warned: yes | matches: yes
preg_split: Internal error | warned: yes | matches: yes
preg_grep: Internal error | warned: yes | matches: yes
