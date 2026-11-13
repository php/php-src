--TEST--
In-pattern (*UTF) forbids \C like the /u modifier
--FILE--
<?php
$patterns = [
    '/(*UTF)\C/',
    '/(*UTF8)a\C/',
    '/(*LIMIT_MATCH=100)(*UTF)a\C/',
    '/\C/u',
];
foreach ($patterns as $pattern) {
    var_dump(preg_match($pattern, "a\u{E4}"));
}

// GH-21134 reproducer using (*UTF) instead of /u
var_dump(preg_match_all("/(*UTF)(.*)\\C/", "\u{E0}", $m), $m);

// \C is still allowed outside UTF mode, and an escaped backslash is not \C
var_dump(preg_match('/a\C/', "a\u{E4}", $m), bin2hex($m[0]));
var_dump(preg_match('/(*UTF)\\\\C/', 'x\C'));
?>
--EXPECTF--

Warning: preg_match(): Compilation failed: using \C is incompatible with UTF-8 mode in %s on line %d
bool(false)

Warning: preg_match(): Compilation failed: using \C is incompatible with UTF-8 mode in %s on line %d
bool(false)

Warning: preg_match(): Compilation failed: using \C is incompatible with UTF-8 mode in %s on line %d
bool(false)

Warning: preg_match(): Compilation failed: using \C is incompatible with the 'u' modifier at offset 2 in %s on line %d
bool(false)

Warning: preg_match_all(): Compilation failed: using \C is incompatible with UTF-8 mode in %s on line %d
bool(false)
NULL
int(1)
string(4) "61c3"
int(1)
