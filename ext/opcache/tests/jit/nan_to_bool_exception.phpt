--TEST--
JIT: NAN to bool coercion warning promoted to exception
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=1205
--EXTENSIONS--
opcache
--FILE--
<?php
set_error_handler(function ($no, $str) {
    throw new Exception($str);
});

function jmpz(float $d) {
    if ($d) {
        echo "side effect\n";
    }
    echo "side effect\n";
}

function jmp_set(float $d) {
    $r = $d ?: 1;
    echo "side effect\n";
    return $r;
}

function isempty(float $d) {
    $r = empty($d);
    echo "side effect\n";
    return $r;
}

foreach (["jmpz", "jmp_set", "isempty"] as $f) {
    try {
        $f(NAN);
    } catch (Exception $e) {
        echo "$f: ", $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
jmpz: unexpected NAN value was coerced to bool
jmp_set: unexpected NAN value was coerced to bool
isempty: unexpected NAN value was coerced to bool
