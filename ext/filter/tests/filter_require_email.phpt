--TEST--
filter_require_var() and FILTER_VALIDATE_EMAIL
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
'e.x.a.m.p.l.e.@example.com',
'firstname.lastname@employee.2something.com',
'-@foo.com',
'foo@-.com',
'foo@bar.123',
'foo@bar.-'
);
foreach ($values as $value) {
	try {
		var_dump(filter_require_var($value, FILTER_VALIDATE_EMAIL));
	} catch (Exception $e) {
		var_dump($e->getMessage());
	}
}

echo "Done\n";
?>
--EXPECT--
string(5) "a@b.c"
string(17) "abuse@example.com"
string(39) "Email validation: Invalid email address"
string(39) "Email validation: Invalid email address"
string(39) "Email validation: Invalid email address"
string(39) "Email validation: Invalid email address"
string(39) "Email validation: Invalid email address"
string(39) "Email validation: Invalid email address"
string(57) "QWERTYUIOPASDFGHJKLZXCVBNM@QWERTYUIOPASDFGHJKLZXCVBNM.NET"
string(39) "Email validation: Invalid email address"
string(42) "firstname.lastname@employee.2something.com"
string(9) "-@foo.com"
string(39) "Email validation: Invalid email address"
string(39) "Email validation: Invalid email address"
string(39) "Email validation: Invalid email address"
Done
