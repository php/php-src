--TEST--
Bug #44214-2 (crash with preg_replace_callback() and global variable)
--FILE--
<?php
$string = 'aaa bbb ccc ddd eee ccc aaa bbb';

$array = array();

function myCallBack( $match ) {
    global $array;
    $array[] = $match[0];
    return 'xxx';
}

var_dump(preg_replace_callback( '`a+`', 'myCallBack', $string));
var_dump($array);
?>
--EXPECT--
unicode(31) "xxx bbb ccc ddd eee ccc xxx bbb"
array(2) {
  [0]=>
  unicode(3) "aaa"
  [1]=>
  unicode(3) "aaa"
}
