--TEST--
serialize() keeps O: for incomplete classes whose properties look like raw C:/E: payloads
--FILE--
<?php
enum Suit { case Hearts; }

$raw = 'x:i:0;a:1:{i:0;s:3:"pwn";};m:a:0:{}';
$untrusted = 'a:2:{i:0;O:11:"ArrayObject":1:{s:33:"__PHP_Incomplete_Class_Serialized";s:' . strlen($raw) . ':"' . $raw . '";}'
    . 'i:1;O:4:"Suit":1:{s:26:"__PHP_Incomplete_Enum_Case";s:6:"Hearts";}}';

$stored = serialize(unserialize($untrusted, ['allowed_classes' => false]));
var_dump($stored === $untrusted);
?>
--EXPECT--
bool(true)
