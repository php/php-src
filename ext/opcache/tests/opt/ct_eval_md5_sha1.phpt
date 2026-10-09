--TEST--
Compile-time evaluation of md5() and sha1()
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
zend_test.observer.enabled=0
--FILE--
<?php
function test() {
    return [
        md5('hello'),
        bin2hex(md5('hello', true)),
        sha1('hello'),
        bin2hex(sha1('hello', true)),
    ];
}
var_dump(test());
?>
--EXPECTF--
$_main:
     ; (lines=6, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %sct_eval_md5_sha1.php:1-12
0000 INIT_FCALL 1 %d string("var_dump")
0001 INIT_FCALL 0 %d string("test")
0002 T0 = DO_UCALL
0003 SEND_VAL T0 1
0004 DO_ICALL
0005 RETURN int(1)

test:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sct_eval_md5_sha1.php:2-9
0000 RETURN array(...)
array(4) {
  [0]=>
  string(32) "5d41402abc4b2a76b9719d911017c592"
  [1]=>
  string(32) "5d41402abc4b2a76b9719d911017c592"
  [2]=>
  string(40) "aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d"
  [3]=>
  string(40) "aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d"
}
