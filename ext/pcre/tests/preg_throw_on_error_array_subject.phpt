--TEST--
PREG_THROW_ON_ERROR: for an array of subjects the flag throws exactly when the unflagged call leaves an error in preg_last_error(), with the same code
--FILE--
<?php

$bad = "\xff";

$cases = [
    'replace bad-first' => [fn() => preg_replace('//u', 'x', [$bad, 'ok'], -1, $c), fn() => preg_replace('//u', 'x', [$bad, 'ok'], -1, $c, PREG_THROW_ON_ERROR)],
    'replace bad-last'  => [fn() => preg_replace('//u', 'x', ['ok', $bad], -1, $c), fn() => preg_replace('//u', 'x', ['ok', $bad], -1, $c, PREG_THROW_ON_ERROR)],
    'filter bad-last'   => [fn() => preg_filter('//u', 'x', ['ok', $bad], -1, $c), fn() => preg_filter('//u', 'x', ['ok', $bad], -1, $c, PREG_THROW_ON_ERROR)],
    'callback bad-last' => [fn() => preg_replace_callback('//u', fn($m) => 'x', ['ok', $bad], -1, $c), fn() => preg_replace_callback('//u', fn($m) => 'x', ['ok', $bad], -1, $c, PREG_THROW_ON_ERROR)],
    'callback_array bad-last' => [fn() => preg_replace_callback_array(['//u' => fn($m) => 'x'], ['ok', $bad], -1, $c), fn() => preg_replace_callback_array(['//u' => fn($m) => 'x'], ['ok', $bad], -1, $c, PREG_THROW_ON_ERROR)],
    'grep bad-first'    => [fn() => preg_grep('//u', [$bad, 'ok']), fn() => preg_grep('//u', [$bad, 'ok'], PREG_THROW_ON_ERROR)],
    'grep bad-last'     => [fn() => preg_grep('//u', ['ok', $bad]), fn() => preg_grep('//u', ['ok', $bad], PREG_THROW_ON_ERROR)],
];

foreach ($cases as $label => [$noflag, $flag]) {
    $noflag();
    $err = preg_last_error();
    $reports = $err !== PREG_NO_ERROR;

    $threw = false;
    $code = PREG_NO_ERROR;
    try {
        $flag();
    } catch (PregException $e) {
        $threw = true;
        $code = $e->getCode();
    }

    $consistent = ($threw === $reports) && (!$threw || $code === $err);
    printf("%s: unflagged reports error=%s, flag throws=%s, consistent=%s\n",
        $label, $reports ? 'yes' : 'no', $threw ? 'yes' : 'no', $consistent ? 'yes' : 'no');
}

?>
--EXPECT--
replace bad-first: unflagged reports error=no, flag throws=no, consistent=yes
replace bad-last: unflagged reports error=yes, flag throws=yes, consistent=yes
filter bad-last: unflagged reports error=yes, flag throws=yes, consistent=yes
callback bad-last: unflagged reports error=yes, flag throws=yes, consistent=yes
callback_array bad-last: unflagged reports error=yes, flag throws=yes, consistent=yes
grep bad-first: unflagged reports error=yes, flag throws=yes, consistent=yes
grep bad-last: unflagged reports error=yes, flag throws=yes, consistent=yes
