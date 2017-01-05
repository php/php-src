--TEST--
Bug #55871 (Interruption in substr_replace()) (PHP7)
--FILE--
<?php
class test1 {
	public function __toString() {
		preg_match('//', '', $GLOBALS['my_var']);
		return '';
	}
}

class test2 {
        public function __toString() {
		$GLOBALS['my_var'] += 0x08048000;
                return '';
        }
}

class test3 {
        public function __toString() {                
                $GLOBALS['my_var'] .= "AAAAAAAA";
                return '';
        }
}

$my_var = str_repeat('A', 40);
$out = substr_replace(array(&$my_var), array(new test1), 40, 0);
var_dump($out, $my_var);
$my_var = str_repeat('A', 40);
$out = substr_replace(array(&$my_var), array(new test2), 40, 0);
var_dump($out, $my_var);
$my_var = str_repeat('A', 40);
$out = substr_replace(array(&$my_var), array(new test3), 40, 0);
var_dump($out, $my_var);
--EXPECTF--
array(1) {
  [0]=>
  string(40) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(40) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
}
int(134512640)
array(1) {
  [0]=>
  string(40) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
}
string(48) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
