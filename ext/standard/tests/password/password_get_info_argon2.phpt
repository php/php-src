--TEST--
Test normal operation of password_get_info() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_get_info not built with Argon2');
?>
--FILE--
<?php
// Basics, testing the array output itself
var_dump(password_get_info('$argon2i$v=19$m=65536,t=3,p=1$SWhIcG5MT21Pc01PbWdVZw$WagZELICsz7jlqOR2YzoEVTWb2oOX1tYdnhZYXxptbU'));
var_dump(password_get_info('$argon2id$v=19$m=1024,t=2,p=2$Zng1U1RHS0h1aUJjbGhPdA$ajQnG5s01Ws1ad8xv+1qGfXF8mYxxWdyul5rBpomuZQ'));

$template    = '$%s$%s$%s$%s$%s';
$b64alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
$defaults    = [
	'v'      => 'v=19',
	'params' => sprintf('m=%s,t=%s,p=%s', PASSWORD_ARGON2_DEFAULT_MEMORY_COST, PASSWORD_ARGON2_DEFAULT_TIME_COST, PASSWORD_ARGON2_DEFAULT_THREADS),
	'salt'   => substr($b64alphabet, 0, 22),
	'digest' => substr($b64alphabet, -43),
];

$valid = [
	'optional version 1.0 excluded' => sprintf(substr($template, 3), 'argon2i', $defaults['params'], $defaults['salt'], $defaults['digest']),
	'optional version 1.0 included' => sprintf($template, 'argon2i', 'v=16', $defaults['params'], $defaults['salt'], $defaults['digest']),
	// Also tests min param values
	'shortest possible hash' => sprintf(substr($template, 3), 'argon2i', 'm=8,t=1,p=1', substr($b64alphabet, 0, 11), substr($b64alphabet, -16)),
	// Also tests theoretical max param values and that all B64 characters are accepted
	'longest possible hash' => sprintf($template, 'argon2id', 'v=19', 'm=4294967295,t=4294967295,p=255', $b64alphabet, str_repeat('F', 86)),
];

foreach ($valid as $name => $hash) {
	echo sprintf("%s: %s\n", $name, (password_get_info($hash)['algoName'] === 'unknown') ? 'REJECTED' : 'OK');
}

echo "OK!"
?>
--EXPECTF--
array(3) {
  ["algo"]=>
  string(7) "argon2i"
  ["algoName"]=>
  string(7) "argon2i"
  ["options"]=>
  array(3) {
    ["memory_cost"]=>
    int(65536)
    ["time_cost"]=>
    int(3)
    ["threads"]=>
    int(1)
  }
}
array(3) {
  ["algo"]=>
  string(8) "argon2id"
  ["algoName"]=>
  string(8) "argon2id"
  ["options"]=>
  array(3) {
    ["memory_cost"]=>
    int(1024)
    ["time_cost"]=>
    int(2)
    ["threads"]=>
    int(2)
  }
}
optional version 1.0 excluded: OK
optional version 1.0 included: OK
shortest possible hash: OK
longest possible hash: OK
OK!
