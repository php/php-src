--TEST--
GH-17727 (JIT SEGV on OOM in dtor when creating backtrace)
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
opcache.jit=1254
fatal_error_backtraces=1
memory_limit=2M
--CREDITS--
arnaud-lb
YuanchengJiang
--FILE--
<?php
class DestructableObject
{
    public function __destruct()
    {
        DestructableObject::__destruct();
    }
}
$_ = new DestructableObject();
?>
--EXPECTF--
Fatal error: Allowed memory size of 2097152 bytes exhausted %s
Stack trace:
#0 %s(%d): DestructableObject->__destruct()
%A
#%d [internal function]: DestructableObject->__destruct()
#%d {main}
