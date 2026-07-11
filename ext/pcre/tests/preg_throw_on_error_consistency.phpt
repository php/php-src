--TEST--
PREG_THROW_ON_ERROR: the exception mirrors preg_last_error()/preg_last_error_msg() byte-for-byte for every error kind
--INI--
pcre.backtrack_limit=1000
pcre.jit=0
--FILE--
<?php

$cases = [
    'bad utf-8 (execution)'       => fn() => preg_match('//u', "\xff", $m, PREG_THROW_ON_ERROR),
    'backtrack limit (execution)' => fn() => preg_match('/(a+)+$/', str_repeat('a', 1000) . 'X', $m, PREG_THROW_ON_ERROR),
    'malformed pattern (compile)' => fn() => preg_match('/[/', 'x', $m, PREG_THROW_ON_ERROR),
];

foreach ($cases as $label => $case) {
    try {
        $case();
        echo "$label: no exception thrown\n";
    } catch (PregException $e) {
        printf("%s: %s | code===preg_last_error()=%s, msg===preg_last_error_msg()=%s\n",
            $label,
            $e->getMessage(),
            $e->getCode() === preg_last_error() ? 'yes' : 'no',
            $e->getMessage() === preg_last_error_msg() ? 'yes' : 'no');
    }
}

?>
--EXPECT--
bad utf-8 (execution): Malformed UTF-8 characters, possibly incorrectly encoded | code===preg_last_error()=yes, msg===preg_last_error_msg()=yes
backtrack limit (execution): Backtrack limit exhausted | code===preg_last_error()=yes, msg===preg_last_error_msg()=yes
malformed pattern (compile): Internal error | code===preg_last_error()=yes, msg===preg_last_error_msg()=yes
