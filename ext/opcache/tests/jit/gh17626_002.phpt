--TEST--
GH-17626: Opline handler corrupted when a root trace is blacklisted at the max_root_traces limit
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_hot_func=2
opcache.jit_hot_loop=255
opcache.jit_hot_return=255
opcache.jit_hot_side_exit=255
opcache.jit_max_root_traces=2
--EXTENSIONS--
opcache
--FILE--
<?php
require __DIR__ . '/gh17626_002.inc';

class ParentA extends GH17626GrandParent { public static function m() { return 'A'; } }
class ParentB extends GH17626GrandParent { public static function m() { return 'B'; } }

trait T {
    public function run(string $s) {
        return parent::m() . $s;
    }
}

class A extends ParentA { use T; }
class B extends ParentB { use T; }

$a = new A;
$b = new B;

$a->run('x');
$a->run('x');
$a->run('x');
echo $a->run('x'), "\n";
echo $b->run('y'), "\n";
echo $b->run('y'), "\n";
echo $a->run('x'), "\n";
?>
--EXPECT--
Ax
By
By
Ax
