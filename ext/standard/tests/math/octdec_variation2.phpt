--TEST--
Test octdec() function : strange literals
--FILE--
<?php

var_dump(octdec('0o'));
var_dump(octdec('0O'));
var_dump(octdec('0'));
var_dump(octdec(''));

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
