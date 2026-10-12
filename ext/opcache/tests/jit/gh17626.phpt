--TEST--
GH-17626: Opline handler corrupted when a root trace is blacklisted at the max_root_traces limit (fails with --repeat 2)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.revalidate_freq=0
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
namespace GH17626;

// In --repeat 2 the callee is recompiled after the first run, so the function
// guard in the trace compiled for caller() fails with ZEND_JIT_EXIT_INVALIDATE.

require __DIR__ . '/gh17626.inc';

function caller(string $s) {
    return gh17626_callee($s) . $s;
}

caller('a');
caller('a');
caller('a');
echo caller('a'), "\n";
echo caller('b'), "\n";

touch(__DIR__ . '/gh17626.inc');
opcache_invalidate(__DIR__ . '/gh17626.inc', true);
?>
--EXPECT--
Aa
Bb
