--TEST--
Test argv passing
--SKIPIF--
<?php
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
}
?>
--PHPDBG--
r
r 1 2 3
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> int(5)
array(5) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(2) "--"
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "2"
  [4]=>
  string(1) "3"
}
[Script ended normally]
prompt> int(4)
array(4) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
  [3]=>
  string(1) "3"
}
[Script ended normally]
prompt> int(5)
array(5) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(2) "--"
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "2"
  [4]=>
  string(1) "3"
}
[Script ended normally]
prompt> 
--ARGS--
1 2 3
--FILE--
<?php

var_dump($argc, $argv);
