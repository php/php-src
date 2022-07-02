--TEST--
Bug #75969: Assertion failure in live range DCE due to block pass misoptimization
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php

// This is required for the segfault
md5('foo');

class Extended_Class {};
$response = array(
    'a' => 'b'
);
new Extended_Class( array(
    'foo' => $response,
    'foo2' => 'bar2'
) );

?>
===DONE===
--EXPECT--
===DONE===
