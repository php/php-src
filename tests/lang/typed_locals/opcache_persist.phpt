--TEST--
Typed local variables: cv_types are preserved through opcache (SHM/file-cache persistence)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
--FILE--
<?php
function typed_locals_func(string $input) {
    int $count = 0;
    $count = 5;
    string $label = "result";
    $label = $input;
    float $ratio = 1.0;
    $ratio = 3.14;
    return [$count, $label, $ratio];
}

$r = typed_locals_func("hello");
var_dump($r[0], $r[1], $r[2]);
?>
--EXPECT--
int(5)
string(5) "hello"
float(3.14)
