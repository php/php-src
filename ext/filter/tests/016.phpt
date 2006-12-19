--TEST--
filter_var() and FILTER_VALIDATE_EMAIL
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$values = Array(
'a@b.c',	
'abuse@example.com',	
'test!.!@#$%^&*@example.com',	
'test@@#$%^&*())).com',	
'test@.com',	
'test@com',	
'@',	
'[]()/@example.com',	
'QWERTYUIOPASDFGHJKLZXCVBNM@QWERTYUIOPASDFGHJKLZXCVBNM.NET',	
);
foreach ($values as $value) {
	var_dump(filter_var($value, FILTER_VALIDATE_EMAIL));
}

echo "Done\n";
?>
--EXPECT--	
string(5) "a@b.c"
string(17) "abuse@example.com"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
string(57) "QWERTYUIOPASDFGHJKLZXCVBNM@QWERTYUIOPASDFGHJKLZXCVBNM.NET"
Done
