--TEST--
GH-25189 (parse_ini_string() ignores everything after a NUL byte)
--FILE--
<?php
echo "raw: ", json_encode(parse_ini_string("a=b\x00c", true, INI_SCANNER_RAW)), "\n";
echo "normal: ", json_encode(parse_ini_string("a=\"b\x00c\"\nd=e")), "\n";
echo "typed: ", json_encode(parse_ini_string("[s]\na=\"x\x00y\"\n[t]\nd=2", true, INI_SCANNER_TYPED)), "\n";
?>
--EXPECT--
raw: {"a":"b\u0000c"}
normal: {"a":"b\u0000c","d":"e"}
typed: {"s":{"a":"x\u0000y"},"t":{"d":2}}
