--TEST--
SplFileObject iterator should stop at EOF
--FILE--
<?php
$o = new SplFileObject(__FILE__);

foreach($o as $key=>$val) {
  var_dump($val);
}
--EXPECT--
string(6) "<?php
"
string(34) "$o = new SplFileObject(__FILE__);
"
string(1) "
"
string(28) "foreach($o as $key=>$val) {
"
string(18) "  var_dump($val);
"
string(2) "}
"
string(0) ""
