--TEST--
GH-20341 (Unicode character classes match consistently with and without JIT)
--SKIPIF--
<?php
if (PCRE_VERSION_MAJOR === 10 && PCRE_VERSION_MINOR >= 45 && PCRE_VERSION_MINOR <= 47) {
    die("skip PCRE2 10.45-10.47 character class regression");
}
?>
--FILE--
<?php
$cases = [
    ['[\x{ff}\x{100}\x{8000}\x{8002}\x{8004}\x{8006}]', "\u{100}"],
    ['[\x{ff}\x{100}\x{8000}\x{8002}\x{8004}\x{8006}\x{8008}]', "\u{100}"],
    ['[\x{ff}\x{101}\x{8000}\x{8002}\x{8004}\x{8006}\x{8008}]', "\u{101}"],
    ['[\x{ff}-\x{100}\x{8000}\x{8002}\x{8004}\x{8006}\x{8008}]', "\u{100}"],
    ['[\x{ff}-\x{101}\x{8000}\x{8002}\x{8004}\x{8006}\x{8008}]', "\u{100}"],
];

foreach ([0, 1] as $jit) {
    ini_set('pcre.jit', (string) $jit);
    echo "JIT=$jit\n";
    foreach ($cases as [$class, $subject]) {
        $pattern = "~^$class\$(?#jit=$jit)~u";
        var_dump(preg_match($pattern, $subject));
        var_dump(preg_match($pattern, "\u{8001}"));
    }
}
?>
--EXPECT--
JIT=0
int(1)
int(0)
int(1)
int(0)
int(1)
int(0)
int(1)
int(0)
int(1)
int(0)
JIT=1
int(1)
int(0)
int(1)
int(0)
int(1)
int(0)
int(1)
int(0)
int(1)
int(0)
