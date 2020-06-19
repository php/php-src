--TEST--
Bug #79710: Reproducible segfault in error_handler during GC involved an SplFileObject
--FILE--
<?php

class Target
{
    public $sfo;
    public function __construct($sfo) {
        $this->sfo = $sfo;
    }
    public function __destruct() {
        // If the SplFileObject is destructed first,
        // underlying FD is no longer valid and will cause error upon calling flock
        $this->sfo->flock(2);
    }
}

class Run
{
    static $sfo;
    static $foo;
    public static function main() {
        // Creation ordering is important for repro
        // $sfo needed to be destructed before $foo.
        Run::$sfo = new SplTempFileObject();
        Run::$foo = new Target(Run::$sfo);
    }
}

Run::main();

?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Object not initialized in %s:%d
Stack trace:
#0 %s(%d): SplFileObject->flock(2)
#1 [internal function]: Target->__destruct()
#2 {main}
  thrown in %s on line %d
