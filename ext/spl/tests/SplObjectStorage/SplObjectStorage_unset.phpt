--TEST--
SplObjectStorage unset and destructor edge cases
--FILE--
<?php
class HasDestructor {
    public function __destruct() {
        echo "In destructor. Should no longer be accessible in \$s:\n";
        var_dump($GLOBALS['s']);

        throw new RuntimeException("thrown from destructor");
    }
}
$o = new stdClass();
$s = new SplObjectStorage();
$s[$o] = new HasDestructor();
try {
    unset($s[$o]);
} catch (Exception $e) {
    echo "Caught: {$e->getMessage()}\n";
}
var_dump($s);
$s[$o] = new HasDestructor();
try {
    $s->offsetUnset($o);
} catch (Exception $e) {
    echo "Caught: {$e->getMessage()}\n";
}

var_dump($s);
?>
--EXPECT--
In destructor. Should no longer be accessible in $s:
object(SplObjectStorage)#2 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
Caught: thrown from destructor
object(SplObjectStorage)#2 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
In destructor. Should no longer be accessible in $s:
object(SplObjectStorage)#2 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
Caught: thrown from destructor
object(SplObjectStorage)#2 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}