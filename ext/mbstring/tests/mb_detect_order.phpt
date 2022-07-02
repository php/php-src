--TEST--
mb_detect_order()
--EXTENSIONS--
mbstring
--INI--
mbstring.language=Japanese
--FILE--
<?php
//$debug = true;
ini_set('include_path', __DIR__);
include_once('common.inc');


// Set order to "auto"
$r = mb_detect_order('auto');
($r === TRUE) ? print "OK_AUTO\n" : print "NG_AUTO\n";
print implode(', ', mb_detect_order()) . "\n";


// Set order by string
$r = mb_detect_order('SJIS,EUC-JP,JIS,UTF-8');
($r === TRUE) ? print "OK_STR\n" : print "NG_STR\n";
print implode(', ', mb_detect_order()) . "\n";


// Set order by array
$a[] = 'ASCII';
$a[] = 'JIS';
$a[] = 'EUC-JP';
$a[] = 'UTF-8';
$r = mb_detect_order($a);
($r === TRUE) ? print "OK_ARRAY\n" : print "NG_ARRAY\n";
print implode(', ', mb_detect_order()) . "\n";

// Set invalid encoding. Should fail.
print "== INVALID PARAMETER ==\n";

try {
    var_dump(mb_detect_order('BAD_NAME'));
} catch (\ValueError $e) {
     echo $e->getMessage() . \PHP_EOL;
}
var_dump(mb_detect_order());

$a[] = 'BAD_NAME';
try {
    var_dump(mb_detect_order($a));
} catch (\ValueError $e) {
     echo $e->getMessage() . \PHP_EOL;
}
var_dump(mb_detect_order());

?>
--EXPECT--
OK_AUTO
ASCII, JIS, UTF-8, EUC-JP, SJIS
OK_STR
SJIS, EUC-JP, JIS, UTF-8
OK_ARRAY
ASCII, JIS, EUC-JP, UTF-8
== INVALID PARAMETER ==
mb_detect_order(): Argument #1 ($encoding) contains invalid encoding "BAD_NAME"
array(4) {
  [0]=>
  string(5) "ASCII"
  [1]=>
  string(3) "JIS"
  [2]=>
  string(6) "EUC-JP"
  [3]=>
  string(5) "UTF-8"
}
mb_detect_order(): Argument #1 ($encoding) contains invalid encoding "BAD_NAME"
array(4) {
  [0]=>
  string(5) "ASCII"
  [1]=>
  string(3) "JIS"
  [2]=>
  string(6) "EUC-JP"
  [3]=>
  string(5) "UTF-8"
}
