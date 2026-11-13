--TEST--
Test normal operation of password_get_info()
--FILE--
<?php
//-=-=-=-
// Test Bcrypt
var_dump(password_get_info('$2y$10$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y'));
// Test Bcrypt Cost
var_dump(password_get_info('$2y$11$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y'));
// Test Bcrypt Invalid Length
var_dump(password_get_info('$2y$11$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100'));
var_dump(password_get_info('$2y$11$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj1000y'));
// Test Non-Bcrypt
var_dump(password_get_info('$1$rasmusle$rISCgZzpwk3UhDidwXvin0'));

// Valid cost boundaries
$suffix = 'MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR.oP7XOR9HD/vjUuOj100y';
foreach ([4, 31] as $cost) {
	$info = password_get_info('$2y$' . sprintf('%02d', $cost) . '$' . $suffix);
	printf("valid %d: %s, cost %d\n", $cost, $info['algoName'], $info['options']['cost']);
}

// Invalid cost grammar and range
$invalidHashes = [
	'non-digit' => '$2y$a0$' . $suffix,
	'malformed separator' => '$2y$10x' . $suffix,
	'below range' => '$2y$03$' . $suffix,
	'above range' => '$2y$32$' . $suffix,
	'missing separator' => '$2y$' . str_repeat('9', 56),
	'malformed salt segment' => '$2y$04$MTIzNDU2Nzg5MDEyMzQ1NEj0NmcAWSLR.oP7XOR9HD/vjUuOj100y',
	'malformed hash segment' => '$2y$04$MTIzNDU2Nzg5MDEyMzQ1Nej0NmcAWSLR+oP7XOR9HD/vjUuOj100y',
	'malicious misalignment' => '$2y$4$MTIzNDU2Nzg5MDEyMzQ1Neej0NmcAWSLR+oP7XOR9HD/vjUuOj100y',
	'minus sign' => '$2y$-4$' . $suffix,
	'plus sign' => '$2y$+4$' . $suffix,
	'newline' => '$2y$'."\n".'4$' . $suffix,
	'space' => '$2y$ 4$' . $suffix,
];

foreach ($invalidHashes as $description => $hash) {
	$info = password_get_info($hash);
	printf("%s: %s, options %d\n", $description, $info['algoName'], count($info['options']));
}

echo "OK!";
?>
--EXPECT--
array(3) {
  ["algo"]=>
  string(2) "2y"
  ["algoName"]=>
  string(6) "bcrypt"
  ["options"]=>
  array(1) {
    ["cost"]=>
    int(10)
  }
}
array(3) {
  ["algo"]=>
  string(2) "2y"
  ["algoName"]=>
  string(6) "bcrypt"
  ["options"]=>
  array(1) {
    ["cost"]=>
    int(11)
  }
}
array(3) {
  ["algo"]=>
  NULL
  ["algoName"]=>
  string(7) "unknown"
  ["options"]=>
  array(0) {
  }
}
array(3) {
  ["algo"]=>
  NULL
  ["algoName"]=>
  string(7) "unknown"
  ["options"]=>
  array(0) {
  }
}
array(3) {
  ["algo"]=>
  NULL
  ["algoName"]=>
  string(7) "unknown"
  ["options"]=>
  array(0) {
  }
}
valid 4: bcrypt, cost 4
valid 31: bcrypt, cost 31
non-digit: unknown, options 0
malformed separator: unknown, options 0
below range: unknown, options 0
above range: unknown, options 0
missing separator: unknown, options 0
malformed salt segment: unknown, options 0
malformed hash segment: unknown, options 0
malicious misalignment: unknown, options 0
minus sign: unknown, options 0
plus sign: unknown, options 0
newline: unknown, options 0
space: unknown, options 0
OK!
