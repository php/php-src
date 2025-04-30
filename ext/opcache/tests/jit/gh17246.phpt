--TEST--
GH-17246 (Nested shm protections cause segfault)
--EXTENSIONS--
opcache
--INI--
opcache.protect_memory=1
opcache.jit_buffer_size=32M
opcache.jit=1254
--FILE--
<?php

class Test
{
    private $field;

    public function __construct()
    {
        $this->field = function() {};
    }

    public function __destruct()
    {
        // Necessary because we need to invoke tracing JIT during destruction
    }
}

for ($i = 0; $i < 10000; ++$i) {
    $obj = new Test();
}

require __DIR__.'/gh17246.inc';

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "NonExistentClass" not found in %s:%d
Stack trace:
#0 %s(%d): require()
#1 {main}
  thrown in %s on line %d
