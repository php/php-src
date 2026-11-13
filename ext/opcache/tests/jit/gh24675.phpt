--TEST--
GH-24675 (JIT buffer corrupted after failing to compile a huge function)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=16M
opcache.jit=function
--FILE--
<?php
$big = __DIR__ . '/gh24675_big.inc';
$next = __DIR__ . '/gh24675_next.inc';

$body = '';
for ($i = 0; $i < 4000; $i++) {
    $body .= "\$r[" . ($i % 32) . "] = \$v + $i;\n";
}
file_put_contents($big, "<?php\nfunction big(array &\$r, int \$v) {\n$body}\n");
file_put_contents($next, "<?php\nfunction next_fn() { return 42; }\n");

$before = opcache_get_status(false)['jit']['buffer_free'];
include $big;
var_dump(opcache_get_status(false)['jit']['buffer_free'] <= $before);

include $next;
var_dump(next_fn());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24675_big.inc');
@unlink(__DIR__ . '/gh24675_next.inc');
?>
--EXPECT--
bool(true)
int(42)
