--TEST--
bcrypt correctly rejects salts containing $
--FILE--
<?php
for ($i = 0; $i < 23; $i++) {
	$salt = '$2y$04$' . str_repeat('0', $i) . '$';
	$result = crypt("foo", $salt);
	var_dump($salt);
	var_dump($result);
	var_dump($result === $salt);
}
?>
--EXPECT--
string(8) "$2y$04$$"
string(2) "*0"
bool(false)
string(9) "$2y$04$0$"
string(2) "*0"
bool(false)
string(10) "$2y$04$00$"
string(2) "*0"
bool(false)
string(11) "$2y$04$000$"
string(2) "*0"
bool(false)
string(12) "$2y$04$0000$"
string(2) "*0"
bool(false)
string(13) "$2y$04$00000$"
string(2) "*0"
bool(false)
string(14) "$2y$04$000000$"
string(2) "*0"
bool(false)
string(15) "$2y$04$0000000$"
string(2) "*0"
bool(false)
string(16) "$2y$04$00000000$"
string(2) "*0"
bool(false)
string(17) "$2y$04$000000000$"
string(2) "*0"
bool(false)
string(18) "$2y$04$0000000000$"
string(2) "*0"
bool(false)
string(19) "$2y$04$00000000000$"
string(2) "*0"
bool(false)
string(20) "$2y$04$000000000000$"
string(2) "*0"
bool(false)
string(21) "$2y$04$0000000000000$"
string(2) "*0"
bool(false)
string(22) "$2y$04$00000000000000$"
string(2) "*0"
bool(false)
string(23) "$2y$04$000000000000000$"
string(2) "*0"
bool(false)
string(24) "$2y$04$0000000000000000$"
string(2) "*0"
bool(false)
string(25) "$2y$04$00000000000000000$"
string(2) "*0"
bool(false)
string(26) "$2y$04$000000000000000000$"
string(2) "*0"
bool(false)
string(27) "$2y$04$0000000000000000000$"
string(2) "*0"
bool(false)
string(28) "$2y$04$00000000000000000000$"
string(2) "*0"
bool(false)
string(29) "$2y$04$000000000000000000000$"
string(2) "*0"
bool(false)
string(30) "$2y$04$0000000000000000000000$"
string(60) "$2y$04$000000000000000000000u2a2UpVexIt9k3FMJeAVr3c04F5tcI8K"
bool(false)
