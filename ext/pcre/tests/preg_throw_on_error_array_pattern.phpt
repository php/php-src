--TEST--
PREG_THROW_ON_ERROR: preg_replace() with an array of patterns stops at the first failing pattern and throws the error preg_last_error() would report
--FILE--
<?php

$cases = [
    'compile' => fn() => @preg_replace(['/a/', '/[/', '/c/'], 'x', 'abc', -1, $c, PREG_THROW_ON_ERROR),
    'exec'    => fn() => preg_replace(['/a/', '//u'], 'x', "\xff", -1, $c, PREG_THROW_ON_ERROR),
];

foreach ($cases as $label => $case) {
    try {
        $case();
        echo "$label: no exception thrown\n";
    } catch (PregException $e) {
        printf("%s: %s | matches: %s\n", $label, $e->getMessage(),
            ($e->getCode() === preg_last_error() && $e->getMessage() === preg_last_error_msg()) ? 'yes' : 'no');
    }
}

?>
--EXPECT--
compile: Internal error | matches: yes
exec: Malformed UTF-8 characters, possibly incorrectly encoded | matches: yes
