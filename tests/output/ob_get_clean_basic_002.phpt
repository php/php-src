--TEST--
Test basic behaviour of ob_get_clean()
--FILE--
<?php
/* 
 * proto bool ob_get_clean(void)
 * Function is implemented in main/output.c
*/ 

ob_start();

echo "Hello World";

$out = ob_get_clean();
$out = strtolower($out);

var_dump($out);
?>
--EXPECT--
string(11) "hello world"
