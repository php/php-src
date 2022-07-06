--TEST--
Exceptions on improper usage of $this
--FILE--
<?php
try {
    $this->foo();
} catch (Error $e) {
    echo "\nException: " . $e->getMessage() . " in " , $e->getFile() . " on line " . $e->getLine() . "\n";
}

$this->foo();
?>
--EXPECTF--
Exception: Using $this when not in object context in %sexception_016.php on line %d

Fatal error: Uncaught Error: Using $this when not in object context in %sexception_016.php:%d
Stack trace:
#0 {main}
  thrown in %sexception_016.php on line %d
