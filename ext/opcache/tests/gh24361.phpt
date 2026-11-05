--TEST--
GH-24361 (opcache_compile_file() disables cycle collection after a compile error)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
var_dump(gc_status()['protected']);
var_dump(@opcache_compile_file(__DIR__ . '/gh24361.inc'));
var_dump(gc_status()['protected']);

$node = new stdClass;
$node->self = $node;
unset($node);
var_dump(gc_collect_cycles());

for ($i = 0; $i < 100000; $i++) {
    $node = new stdClass;
    $node->self = $node;
}
unset($node);
var_dump(gc_status()['runs'] > 1);
gc_collect_cycles();
var_dump(gc_status()['collected']);

ini_set('opcache.enable', '0');
gc_disable();
var_dump(@opcache_compile_file(__DIR__ . '/gh24361.inc'));
var_dump(gc_status()['protected']);
var_dump(gc_enabled());
$node = new stdClass;
$node->self = $node;
unset($node);
var_dump(gc_collect_cycles());
?>
--EXPECTF--
bool(false)

Fatal error: 'break' not in the 'loop' or 'switch' context in %sgh24361.inc on line 2
bool(false)
bool(false)
int(1)
bool(true)
int(100001)

Fatal error: 'break' not in the 'loop' or 'switch' context in %sgh24361.inc on line 2
bool(false)
bool(false)
bool(false)
int(1)
