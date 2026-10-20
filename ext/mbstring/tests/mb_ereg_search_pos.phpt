--TEST--
mb_ereg_search_pos() # a test for the basic function of mb_ereg_search_pos
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_regex_encoding")) die("skip mb_regex_encoding() is not defined");
?>
--FILE--
<?php
mb_regex_encoding('iso-8859-1');
$test_str = 'Iñtërnâtiônàlizætiøn';

if(mb_ereg_search_init($test_str))
{
    $val=mb_ereg_search_pos("nâtiôn");

    var_dump($val);

}
else{
    var_dump("false");
}
?>
--EXPECTF--
Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_pos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
array(2) {
  [0]=>
  int(5)
  [1]=>
  int(6)
}
