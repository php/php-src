--TEST--
Exceptions on improper access to string
--FILE--
<?php
$s = "ABC";
try {
    $s[] = "D";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), ' in ', $e->getFile(), ' on line ', $e->getLine(), "\n";
}

$s[] = "D";
?>
--EXPECTF--
Error: [] operator not supported for strings in %sexception_015.php on line %d

Fatal error: Uncaught Error: [] operator not supported for strings in %sexception_015.php:%d
Stack trace:
#0 {main}
  thrown in %sexception_015.php on line %d
