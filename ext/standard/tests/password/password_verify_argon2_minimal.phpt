--TEST--
Test normal operation of password_verify() with Argon2i and Argon2id -- minimal
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_verify not built with Argon2');
?>
--FILE--
<?php
// Generated with minimal settings recommended by OWASP as of November 2026:
// https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html#argon2id
// Keeping these minimal for CI purposes too; more in password_verify_argon2_extensive.phpt
$minimal = [
	['hash' => '$argon2i$v=19$m=12288,t=3,p=1$bzFZemY2Z2xwdDIxc0F1cg$Dsogk3TkrXQxb9+vcZb8/8/IcHy71faoZtFk9Ej76AA', 'password' => 'password'],
	['hash' => '$argon2id$v=19$m=47104,t=1,p=1$amxxbjAxZUV2NlQ3U2JzdQ$oac38S6CrOni1SC9RR5PaMiBLAyFHjkYgUAIVVVpU1k', 'password' => 'password'],
];

foreach ($minimal as $test) {
	var_dump(password_verify($test['password'], $test['hash']));
	var_dump(password_verify(strtoupper($test['password']), $test['hash']));
}
echo "OK!";
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
OK!
