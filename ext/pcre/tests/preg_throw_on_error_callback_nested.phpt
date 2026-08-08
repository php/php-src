--TEST--
PREG_THROW_ON_ERROR: a nested preg_*() error inside a replacement callback does not make the outer (successful) call throw
--FILE--
<?php

$polluter = function ($m) {
    @preg_match('//u', "\xff");
    return strtoupper($m[0]);
};

$r = preg_replace_callback('/\w/', $polluter, 'ab', -1, $c, PREG_THROW_ON_ERROR);
printf("callback single: %s (count=%d)\n", $r, $c);

$r = preg_replace_callback('/\w/', $polluter, ['ab', 'cd'], -1, $c, PREG_THROW_ON_ERROR);
printf("callback array: %s (count=%d)\n", implode(',', $r), $c);

$r = preg_replace_callback_array(['/\w/' => $polluter], 'xy', -1, $c, PREG_THROW_ON_ERROR);
printf("callback_array: %s (count=%d)\n", $r, $c);

try {
    preg_replace_callback('//u', fn($m) => 'x', "\xff", -1, $c, PREG_THROW_ON_ERROR);
    echo "outer error: no throw\n";
} catch (PregException $e) {
    echo "outer error: threw ", $e->getMessage(), "\n";
}

try {
    preg_replace_callback('/\w/', function ($m) {
        throw new RuntimeException('from callback');
    }, 'a', -1, $c, PREG_THROW_ON_ERROR);
    echo "user exception: no throw\n";
} catch (RuntimeException $e) {
    echo "user exception: ", $e->getMessage(), "\n";
}

?>
--EXPECT--
callback single: AB (count=2)
callback array: AB,CD (count=4)
callback_array: XY (count=2)
outer error: threw Malformed UTF-8 characters, possibly incorrectly encoded
user exception: from callback
