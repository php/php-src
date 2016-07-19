--TEST--
Bug #72264 (base64_decode $strict fails with whitespace between padding)
--FILE--
<?php
var_dump(base64_decode("VV= =", true));
--EXPECT--
string(1) "U"
