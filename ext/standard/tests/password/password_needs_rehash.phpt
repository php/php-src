--TEST--
Test normal operation of password_needs_rehash()
--FILE--
<?php
//-=-=-=-

// Invalid Hash, always rehash
var_dump(password_needs_rehash('', PASSWORD_BCRYPT));
var_dump(password_needs_rehash('', 1));
var_dump(password_needs_rehash('', '2y'));

// Valid, as it's an unknown algorithm
var_dump(password_needs_rehash('', PASSWORD_DEFAULT));
var_dump(password_needs_rehash('', 0));
var_dump(password_needs_rehash('', NULL));

// Valid with cost the same
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 10)));

// Valid with cost the same, additional params
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 10, 'foo' => 3)));

// Invalid, different (lower) cost
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 0)));

// Invalid, different (higher) cost
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 11)));

// Valid with cost the default
$cost = str_pad(PASSWORD_BCRYPT_DEFAULT_COST, 2, '0', STR_PAD_LEFT);
var_dump(password_needs_rehash('$2y$'.$cost.'$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT));

// Should Issue Needs Rehash, Since Foo is cast to 0...
var_dump(password_needs_rehash('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y', PASSWORD_BCRYPT, array('cost' => 'foo')));

// Valid cost boundaries
$suffix = 'MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y';
$costCases = [
	'valid lower boundary' => ['$2y$04$' . $suffix, 4],
	'valid upper boundary' => ['$2y$31$' . $suffix, 31],
	'non-digit' => ['$2y$a0$' . $suffix, 12],
	'malformed separator' => ['$2y$10x' . $suffix, 10],
	'below range' => ['$2y$03$' . $suffix, 3],
	'above range' => ['$2y$32$' . $suffix, 32],
];
foreach ($costCases as $description => $case) {
	echo $description, ': ';
	var_dump(password_needs_rehash($case[0], PASSWORD_BCRYPT, ['cost' => $case[1]]));
}

// CRYPT_MD5
var_dump(password_needs_rehash(crypt('Example', '$1$'), PASSWORD_DEFAULT));

// CRYPT_SHA512 with 5000
var_dump(password_needs_rehash(crypt('Example', '$6$rounds=5000$aa$'), PASSWORD_DEFAULT));

echo "OK!";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
valid lower boundary: bool(false)
valid upper boundary: bool(false)
non-digit: bool(true)
malformed separator: bool(true)
below range: bool(true)
above range: bool(true)
bool(true)
bool(true)
OK!
