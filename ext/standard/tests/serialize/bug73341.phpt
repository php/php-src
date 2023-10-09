--TEST--
Bug #73144 (Use-afte-free in ArrayObject Deserialization)
--FILE--
<?php
try {
$token = 'a:2:{i:0;O:1:"0":2:{s:1:"0";i:0;s:1:"0";a:1:{i:0;C:11:"ArrayObject":7:{x:i:0;r}';
$obj = unserialize($token);
} catch(Exception $e) {
    echo $e->getMessage()."\n";
}

try {
$token = 'a:2:{i:0;O:1:"0":2:0s:1:"0";i:0;s:1:"0";a:1:{i:0;C:11:"ArrayObject":7:{x:i:0;r}';
$obj = unserialize($token);
} catch(Exception $e) {
    echo $e->getMessage()."\n";
}

try {
$inner = 'x:i:1;O:8:"stdClass":1:{};m:a:0:{}';
$exploit = 'C:11:"ArrayObject":'.strlen($inner).':{'.$inner.'}';
unserialize($exploit);
} catch(Exception $e) {
    echo $e->getMessage()."\n";
}
?>
--EXPECTF--
Error at offset 6 of 7 bytes

Warning: unserialize(): Error at offset 19 of 79 bytes in %s on line %d

Warning: ArrayObject::unserialize(): Unexpected end of serialized data in %sbug73341.php on line %d
Error at offset 24 of 34 bytes
