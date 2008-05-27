--TEST--
abusing pcre
--FILE--
<?php

var_dump(preg_match_all('/((?:(?:unsigned|struct)\s+)?\w+)(?:\s*(\*+)\s+|\s+(\**))(\w+(?:\[\s*\w*\s*\])?)\s*(?:(=)[^,;]+)?((?:\s*,\s*\**\s*\w+(?:\[\s*\w*\s*\])?\s*(?:=[^,;]+)?)*)\s*;/S', 'unsigned int xpto = 124; short a, b;', $m, PREG_SET_ORDER));
var_dump($m);

var_dump(preg_match_all('/(?:\([^)]+\))?(&?)([\w>.()-]+(?:\[\w+\])?)\s*,?((?:\)*\s*=)?)/S', '&a, b, &c', $m, PREG_SET_ORDER));
var_dump($m);

var_dump(preg_match_all('/zend_parse_parameters(?:_ex\s*\([^,]+,[^,]+|\s*\([^,]+),\s*"([^"]*)"\s*,\s*([^{;]*)/S', 'zend_parse_parameters( 0, "addd|s/", a, b, &c);', $m, PREG_SET_ORDER | PREG_OFFSET_CAPTURE));
var_dump($m);

var_dump(preg_replace(array('@//.*@S', '@/\*.*\*/@SsUe'), array('', 'preg_replace("/[^\r\n]+/S", "", \'$0\')'), "hello\n//x \n/*\ns\n*/"));

var_dump(preg_split('/PHP_(?:NAMED_)?(?:FUNCTION|METHOD)\s*\((\w+(?:,\s*\w+)?)\)/S', "PHP_FUNCTION(s, preg_match)\n{\nlalala", -1, PREG_SPLIT_DELIM_CAPTURE | PREG_SPLIT_OFFSET_CAPTURE));
?>
--EXPECT--
int(2)
array(2) {
  [0]=>
  array(7) {
    [0]=>
    unicode(24) "unsigned int xpto = 124;"
    [1]=>
    unicode(12) "unsigned int"
    [2]=>
    unicode(0) ""
    [3]=>
    unicode(0) ""
    [4]=>
    unicode(4) "xpto"
    [5]=>
    unicode(1) "="
    [6]=>
    unicode(0) ""
  }
  [1]=>
  array(7) {
    [0]=>
    unicode(11) "short a, b;"
    [1]=>
    unicode(5) "short"
    [2]=>
    unicode(0) ""
    [3]=>
    unicode(0) ""
    [4]=>
    unicode(1) "a"
    [5]=>
    unicode(0) ""
    [6]=>
    unicode(3) ", b"
  }
}
int(3)
array(3) {
  [0]=>
  array(4) {
    [0]=>
    unicode(3) "&a,"
    [1]=>
    unicode(1) "&"
    [2]=>
    unicode(1) "a"
    [3]=>
    unicode(0) ""
  }
  [1]=>
  array(4) {
    [0]=>
    unicode(2) "b,"
    [1]=>
    unicode(0) ""
    [2]=>
    unicode(1) "b"
    [3]=>
    unicode(0) ""
  }
  [2]=>
  array(4) {
    [0]=>
    unicode(2) "&c"
    [1]=>
    unicode(1) "&"
    [2]=>
    unicode(1) "c"
    [3]=>
    unicode(0) ""
  }
}
int(1)
array(1) {
  [0]=>
  array(3) {
    [0]=>
    array(2) {
      [0]=>
      unicode(46) "zend_parse_parameters( 0, "addd|s/", a, b, &c)"
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      unicode(7) "addd|s/"
      [1]=>
      int(27)
    }
    [2]=>
    array(2) {
      [0]=>
      unicode(9) "a, b, &c)"
      [1]=>
      int(37)
    }
  }
}
unicode(9) "hello



"
array(3) {
  [0]=>
  array(2) {
    [0]=>
    unicode(0) ""
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    unicode(13) "s, preg_match"
    [1]=>
    int(13)
  }
  [2]=>
  array(2) {
    [0]=>
    unicode(9) "
{
lalala"
    [1]=>
    int(27)
  }
}
