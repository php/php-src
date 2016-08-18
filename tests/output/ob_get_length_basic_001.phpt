--TEST--
Test return type and value, as well as basic behaviour, of ob_get_length()
--FILE--
<?php
/* 
 * proto int ob_get_length(void)
 * Function is implemented in main/output.c
*/ 

echo "No output buffers\n";
var_dump(ob_get_length());

ob_start();
var_dump(ob_get_length());
echo "hello\n";
var_dump(ob_get_length());
ob_flush();
$value = ob_get_length();
echo "hello\n";
ob_clean();
var_dump(ob_get_length());
var_dump($value);
ob_end_flush();

echo "No output buffers\n";
var_dump(ob_get_length());
?>
--EXPECTF--
No output buffers
bool(false)
int(0)
hello
int(13)
int(0)
int(0)
No output buffers
bool(false)
