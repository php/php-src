--TEST--
ob_start() chunk_size: confirm buffer is flushed after any output call that causes its length to equal or exceed chunk_size. 
--INI--
opcache.optimization_level=0
--FILE--
<?php
/* 
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/ 
// In HEAD, $chunk_size value of 1 should not have any special behaviour (http://marc.info/?l=php-internals&m=123476465621346&w=2).
function callback($string) {
	global $callback_invocations;
	$callback_invocations++;
	$len = strlen($string);
	return "f[call:$callback_invocations; len:$len]$string\n";
}

for ($cs=-1; $cs<10; $cs++) {
  echo "\n----( chunk_size: $cs, output append size: 1 )----\n";
  $callback_invocations=0;
  ob_start('callback', $cs);
  echo '1'; echo '2'; echo '3'; echo '4'; echo '5'; echo '6'; echo '7'; echo '8';
  ob_end_flush();
}

for ($cs=-1; $cs<10; $cs++) {
  echo "\n----( chunk_size: $cs, output append size: 4 )----\n";
  $callback_invocations=0;
  ob_start('callback', $cs);
  echo '1234'; echo '5678';
  ob_end_flush();
}

?>
--EXPECTF--

----( chunk_size: -1, output append size: 1 )----
f[call:1; len:8]12345678

----( chunk_size: 0, output append size: 1 )----
f[call:1; len:8]12345678

----( chunk_size: 1, output append size: 1 )----
f[call:1; len:1]1
f[call:2; len:1]2
f[call:3; len:1]3
f[call:4; len:1]4
f[call:5; len:1]5
f[call:6; len:1]6
f[call:7; len:1]7
f[call:8; len:1]8
f[call:9; len:0]

----( chunk_size: 2, output append size: 1 )----
f[call:1; len:2]12
f[call:2; len:2]34
f[call:3; len:2]56
f[call:4; len:2]78
f[call:5; len:0]

----( chunk_size: 3, output append size: 1 )----
f[call:1; len:3]123
f[call:2; len:3]456
f[call:3; len:2]78

----( chunk_size: 4, output append size: 1 )----
f[call:1; len:4]1234
f[call:2; len:4]5678
f[call:3; len:0]

----( chunk_size: 5, output append size: 1 )----
f[call:1; len:5]12345
f[call:2; len:3]678

----( chunk_size: 6, output append size: 1 )----
f[call:1; len:6]123456
f[call:2; len:2]78

----( chunk_size: 7, output append size: 1 )----
f[call:1; len:7]1234567
f[call:2; len:1]8

----( chunk_size: 8, output append size: 1 )----
f[call:1; len:8]12345678
f[call:2; len:0]

----( chunk_size: 9, output append size: 1 )----
f[call:1; len:8]12345678

----( chunk_size: -1, output append size: 4 )----
f[call:1; len:8]12345678

----( chunk_size: 0, output append size: 4 )----
f[call:1; len:8]12345678

----( chunk_size: 1, output append size: 4 )----
f[call:1; len:4]1234
f[call:2; len:4]5678
f[call:3; len:0]

----( chunk_size: 2, output append size: 4 )----
f[call:1; len:4]1234
f[call:2; len:4]5678
f[call:3; len:0]

----( chunk_size: 3, output append size: 4 )----
f[call:1; len:4]1234
f[call:2; len:4]5678
f[call:3; len:0]

----( chunk_size: 4, output append size: 4 )----
f[call:1; len:4]1234
f[call:2; len:4]5678
f[call:3; len:0]

----( chunk_size: 5, output append size: 4 )----
f[call:1; len:8]12345678
f[call:2; len:0]

----( chunk_size: 6, output append size: 4 )----
f[call:1; len:8]12345678
f[call:2; len:0]

----( chunk_size: 7, output append size: 4 )----
f[call:1; len:8]12345678
f[call:2; len:0]

----( chunk_size: 8, output append size: 4 )----
f[call:1; len:8]12345678
f[call:2; len:0]

----( chunk_size: 9, output append size: 4 )----
f[call:1; len:8]12345678
