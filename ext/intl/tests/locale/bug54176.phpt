--TEST--
Bug #54176 (locale_compose accepts more than 3 extlang)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die("skip intl extension not available");
?>
--FILE--
<?php
$arr = [
    'language'=>'en',
    'script'  =>'Hans',
    'region'  =>'CN',
    'extlang' => ['a','b','c','d','e','f'],
];
var_dump(locale_compose($arr));
$arr = [
    'language'=>'en',
    'script'  =>'Hans',
    'region'  =>'CN',
    'extlang0' => 'a',
    'extlang1' => 'b',
    'extlang2' => 'c',
    'extlang3' => 'd',
];
var_dump(locale_compose($arr));
?>
--EXPECT--
string(16) "en_a_b_c_Hans_CN"
string(16) "en_a_b_c_Hans_CN"
