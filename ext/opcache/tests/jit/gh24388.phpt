--TEST--
GH-24388 (Tracing JIT: segfault when a trace runs out of side exits)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
$file = __DIR__ . "/gh24388.inc";

$code = '<?php function f(int $k, int $a) { switch ($k) {';
for ($i = 0; $i < 507; $i++) {
    $code .= "case $i: \$l = $i; break;";
}
$code .= '} return ++$a; }';
$code .= 'for ($i = 0; $i < 200; $i++) { $a = 3; f(0, $a); }';
file_put_contents($file, $code);

include $file;
?>
DONE
--CLEAN--
<?php
@unlink(__DIR__ . "/gh24388.inc");
?>
--EXPECT--
DONE
