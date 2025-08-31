--TEST--
Bug #70944 (try{ } finally{} can create infinite chains of exceptions)
--FILE--
<?php
try {
    $e = new Exception("Foo");
    try {
        throw  new Exception("Bar", 0, $e);
    } finally {
        throw $e;
    }
} catch (Exception $e) {
    var_dump((string)$e);
}

try {
    $e = new Exception("Foo");
    try {
        throw new Exception("Bar", 0, $e);
    } finally {
        throw new Exception("Dummy", 0, $e);
    }
} catch (Exception $e) {
    var_dump((string)$e);
}
?>
--EXPECTF--
string(%d) "Exception: Foo in %sbug70944.php:%d
Stack trace:
#0 {main}"
string(%d) "Exception: Foo in %sbug70944.php:%d
Stack trace:
#0 {main}

Next Exception: Dummy in %sbug70944.php:%d
Stack trace:
#0 {main}"
