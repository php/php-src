--TEST--
Use of double-port in fsockopen()
--FILE--
<?php

$try = [
  '127.0.0.1:80',
  'tcp://127.0.0.1:80',
  '[::1]:80',
  'tcp://[::1]:80',
  'localhost:80',
  'tcp://localhost:80',
];

foreach ($try as $addr) {
  echo "== $addr ==\n";
  var_dump(@fsockopen($addr, 81, $errno, $errstr), $errstr);
}
--EXPECTF--
== 127.0.0.1:80 ==
bool(false)
string(41) "Failed to parse address "127.0.0.1:80:81""
== tcp://127.0.0.1:80 ==
bool(false)
string(41) "Failed to parse address "127.0.0.1:80:81""
== [::1]:80 ==
bool(false)
string(37) "Failed to parse address "[::1]:80:81""
== tcp://[::1]:80 ==
bool(false)
string(37) "Failed to parse address "[::1]:80:81""
== localhost:80 ==
bool(false)
string(41) "Failed to parse address "localhost:80:81""
== tcp://localhost:80 ==
bool(false)
string(41) "Failed to parse address "localhost:80:81""