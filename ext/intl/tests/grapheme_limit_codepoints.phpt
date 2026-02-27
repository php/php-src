--TEST--
grapheme_limit_codepoints() function test
--EXTENSIONS--
intl
--FILE--
<?php
$f = "あい👨‍👨‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦‍👦うえお";
var_dump(grapheme_limit_codepoints($f));
var_dump(grapheme_strlen($f));
$f = "あいうえお👨‍👨‍👦";
var_dump(grapheme_limit_codepoints($f));
var_dump(grapheme_strlen($f));
$f = "あいうえおH̵̛͕̞̦̰̜͍̰̥̟͆̏͂̌͑ͅä̷͔̟͓̬̯̟͍̭͉͈̮͙̣̯̬͚̞̭̍̀̾͠m̴̡̧̛̝̯̹̗̹̤̲̺̟̥̈̏͊̔̑̍͆̌̀̚͝͝b̴̢̢̫̝̠̗̼̬̻̮̺̭͔̘͑̆̎̚ư̵̧̡̥̙̭̿̈̀̒̐̊͒͑r̷̡̡̲̼̖͎̫̮̜͇̬͌͘g̷̹͍͎̬͕͓͕̐̃̈́̓̆̚͝ẻ̵̡̼̬̥̹͇̭͔̯̉͛̈́̕r̸̮̖̻̮̣̗͚͖̝̂͌̾̓̀̿̔̀͋̈́͌̈́̋͜👨‍👨‍👦";
var_dump(grapheme_limit_codepoints($f));
var_dump(grapheme_strlen($f));
$f = "ཧྐྵྨླྺྼྻྂ";
var_dump(grapheme_limit_codepoints($f));
var_dump(grapheme_strlen($f));
?>
--EXPECT--
bool(false)
int(6)
bool(true)
int(6)
bool(true)
int(15)
bool(true)
int(1)
