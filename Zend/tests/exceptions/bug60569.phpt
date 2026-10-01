--TEST--
Bug #60569 (Nullbyte truncates Exception $message.)
--FILE--
<?php
try {
    $msg = "Some error \x00 message";
    throw new Exception($msg);
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($msg);
}
?>
--EXPECTF--
Exception: Some error %0 message
string(20) "Some error %0 message"
