--TEST--
GH-15981 (Segfault with frameless jumps and minimal JIT)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1111
--FILE--
<?php

namespace NS { // Namespace is important to reproduce the issue
    class Tester {
        static public function findExecutable(): string {
            return dirname(__DIR__);
        }
    }
}

namespace {
    var_dump(NS\Tester::findExecutable());
}

?>
--EXPECTF--
string(%d) "%s"
