--TEST--
Bug #52971 (PCRE-Meta-Characters not working with utf-8)
--SKIPIF--
<?php if ((double)PCRE_VERSION < 8.1) die('skip PCRE_VERSION >= 8.1 is required!'); ?>
--FILE--
<?php

$message = 'Der ist ein Süßwasserpool Süsswasserpool ... verschiedene Wassersportmöglichkeiten bei ...';

$pattern = '/\bwasser/iu';
preg_match_all($pattern, $message, $match, PREG_OFFSET_CAPTURE);
var_dump($match);

$pattern = '/[^\w]wasser/iu';
preg_match_all($pattern, $message, $match, PREG_OFFSET_CAPTURE);
var_dump($match);

?>
--EXPECTF--
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(2) {
      [0]=>
      string(6) "Wasser"
      [1]=>
      int(61)
    }
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(2) {
      [0]=>
      string(7) " Wasser"
      [1]=>
      int(60)
    }
  }
}
