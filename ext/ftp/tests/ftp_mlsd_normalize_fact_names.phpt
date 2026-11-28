--TEST--
ftp_mlsd() optionally normalizes fact names without changing filenames or values
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');

$original = ftp_mlsd($ftp, 'normalize');
$explicit = ftp_mlsd($ftp, 'normalize', false);
$normalized = ftp_mlsd($ftp, 'normalize', true);

var_dump($original === $explicit);
var_dump((new ReflectionFunction('ftp_mlsd'))->getParameters()[2]->getName());
var_dump(array_keys($original[0]));
var_dump($normalized[0]);
var_dump($normalized[1]);
var_dump($original[2]['name'], $original[2]['Name'], $normalized[2]['name']);
var_dump($original[3]['name'], $normalized[3]['name']);
var_dump($original[4]['Type'], $original[4]['type'], $normalized[4]['type']);
var_dump($normalized[5]['name'], bin2hex(array_keys($normalized[5])[1]), $normalized[5]["x.\xC4foo"]);
var_dump(ftp_mlsd($ftp, 'emptydir', true));
var_dump(ftp_mlsd($ftp, 'no_exists/', true));
$warnings = [];
set_error_handler(static function (int $severity, string $message) use (&$warnings): bool {
    $warnings[] = $message;
    return true;
});
$malformed = ftp_mlsd($ftp, '.', true);
restore_error_handler();
var_dump(count($malformed), count($warnings));

ftp_close($ftp);
?>
--EXPECT--
bool(true)
string(20) "normalize_fact_names"
array(5) {
  [0]=>
  string(4) "name"
  [1]=>
  string(4) "Type"
  [2]=>
  string(4) "SIZE"
  [3]=>
  string(6) "Modify"
  [4]=>
  string(9) "UNIX.mode"
}
array(5) {
  ["name"]=>
  string(19) "mixed Case file.txt"
  ["type"]=>
  string(4) "FiLe"
  ["size"]=>
  string(5) "51574"
  ["modify"]=>
  string(14) "20240605091325"
  ["unix.mode"]=>
  string(4) "0644"
}
array(3) {
  ["name"]=>
  string(9) "lower.txt"
  ["type"]=>
  string(3) "DIR"
  ["size"]=>
  string(1) "5"
}
string(10) "actual.txt"
string(5) "wrong"
string(10) "actual.txt"
string(5) "wrong"
string(11) "actual2.txt"
string(5) "THIRD"
string(6) "SeCoNd"
string(5) "THIRD"
string(12) "nonascii.txt"
string(12) "782ec4666f6f"
string(5) "VaLuE"
array(0) {
}
bool(false)
int(4)
int(3)
