--TEST--
Bug #72152 (base64_decode $strict fails to detect null byte)
--FILE--
<?php
var_dump(base64_decode("\x00", true));
var_dump(base64_decode("\x00VVVV", true));
var_dump(base64_decode("VVVV\x00", true));
--EXPECT--
bool(false)
bool(false)
bool(false)
