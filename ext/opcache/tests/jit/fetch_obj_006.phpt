--TEST--
JIT: FETCH_OBJ 006
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
class Test {
    readonly array $prop;
}

$test = new Test;

try {
    throw new Exception;
} catch (Exception) {}

$appendProp2 = (function() {
    $this->prop[] = 1;
})->bindTo($test, Test::class);
$appendProp2();
$appendProp2();
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify readonly property Test::$prop in %sfetch_obj_006.php:13
Stack trace:
#0 %sfetch_obj_006.php(16): Test->{closure}()
#1 {main}
  thrown in %sfetch_obj_006.php on line 13