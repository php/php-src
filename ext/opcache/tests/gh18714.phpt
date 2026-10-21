--TEST--
GH-18714 (opcache_compile_file() breaks class hoisting)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
$hoisted = __DIR__ . '/gh18714_hoisted.inc';
$plain = __DIR__ . '/gh18714_plain.inc';

file_put_contents($hoisted, <<<'PHP'
<?php
$x = new GH18714Hoisted();
echo get_class($x), "\n";
class GH18714Hoisted {}
PHP);

file_put_contents($plain, <<<'PHP'
<?php
class GH18714Plain {}
interface GH18714Iface {}
echo get_class(new GH18714Plain()), "\n";
PHP);

var_dump(opcache_compile_file($hoisted));
require_once $hoisted;

var_dump(opcache_compile_file($plain));
require_once $plain;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh18714_hoisted.inc');
@unlink(__DIR__ . '/gh18714_plain.inc');
?>
--EXPECT--
bool(true)
GH18714Hoisted
bool(true)
GH18714Plain
