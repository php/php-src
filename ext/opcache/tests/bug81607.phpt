--TEST--
Bug #81607: CE_CACHE allocation with concurrent access
--EXTENSIONS--
opcache
pcntl
--INI--
opcache.enable_cli=1
--FILE--
<?php

$pid = pcntl_fork();
if ($pid == 0) {
    // Child: Declare class FooBar {} to allocate CE cache slot.
    require __DIR__ . '/bug81607.inc';
} else if ($pid > 0) {
    pcntl_wait($status);
    var_dump(new FooBar);
} else {
    echo "pcntl_fork() failed\n";
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "FooBar" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
