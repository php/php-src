--TEST--
token_get_all()
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--INI--
short_open_tag=1
--FILE--
<?php

$strings = array(
	'<? echo 1; if (isset($a)) print $a+1; $a++; $a--; $a == 2; $a === 2; endif; ?>',
	'<?php switch($a) { case 1: break; default: break; } while($a) { exit; } ?>',
	'<? /* comment */ if (1 || 2) { } $a = 2 | 1; $b = 3^2; $c = 4&2; ?>',
	/* feel free to add more yourself */
	'wrong syntax here'
);

foreach ($strings as $s) {
	var_dump(token_get_all($s));
}

echo "Done\n";
?>
--EXPECTF--
array(49) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "<?"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
    [2]=>
    int(1)
  }
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
  [5]=>
  string(1) ";"
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "if"
    [2]=>
    int(1)
  }
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [9]=>
  string(1) "("
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "isset"
    [2]=>
    int(1)
  }
  [11]=>
  string(1) "("
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [13]=>
  string(1) ")"
  [14]=>
  string(1) ")"
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "print"
    [2]=>
    int(1)
  }
  [17]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [19]=>
  string(1) "+"
  [20]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
  [21]=>
  string(1) ";"
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "++"
    [2]=>
    int(1)
  }
  [25]=>
  string(1) ";"
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [27]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [28]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "--"
    [2]=>
    int(1)
  }
  [29]=>
  string(1) ";"
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [31]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [32]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "=="
    [2]=>
    int(1)
  }
  [34]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [35]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(1)
  }
  [36]=>
  string(1) ";"
  [37]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [38]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [39]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [40]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "==="
    [2]=>
    int(1)
  }
  [41]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [42]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(1)
  }
  [43]=>
  string(1) ";"
  [44]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [45]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "endif"
    [2]=>
    int(1)
  }
  [46]=>
  string(1) ";"
  [47]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [48]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
array(37) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "switch"
    [2]=>
    int(1)
  }
  [2]=>
  string(1) "("
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [4]=>
  string(1) ")"
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [6]=>
  string(1) "{"
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "case"
    [2]=>
    int(1)
  }
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
  [11]=>
  string(1) ":"
  [12]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "break"
    [2]=>
    int(1)
  }
  [14]=>
  string(1) ";"
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [16]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "default"
    [2]=>
    int(1)
  }
  [17]=>
  string(1) ":"
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "break"
    [2]=>
    int(1)
  }
  [20]=>
  string(1) ";"
  [21]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [22]=>
  string(1) "}"
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [24]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "while"
    [2]=>
    int(1)
  }
  [25]=>
  string(1) "("
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [27]=>
  string(1) ")"
  [28]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [29]=>
  string(1) "{"
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [31]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "exit"
    [2]=>
    int(1)
  }
  [32]=>
  string(1) ";"
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [34]=>
  string(1) "}"
  [35]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [36]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
array(48) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "<?"
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(13) "/* comment */"
    [2]=>
    int(1)
  }
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "if"
    [2]=>
    int(1)
  }
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [6]=>
  string(1) "("
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [9]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "||"
    [2]=>
    int(1)
  }
  [10]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [11]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(1)
  }
  [12]=>
  string(1) ")"
  [13]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [14]=>
  string(1) "{"
  [15]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [16]=>
  string(1) "}"
  [17]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [18]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
    [2]=>
    int(1)
  }
  [19]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [20]=>
  string(1) "="
  [21]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [22]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(1)
  }
  [23]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [24]=>
  string(1) "|"
  [25]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [26]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
  [27]=>
  string(1) ";"
  [28]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [29]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
    [2]=>
    int(1)
  }
  [30]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [31]=>
  string(1) "="
  [32]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [33]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "3"
    [2]=>
    int(1)
  }
  [34]=>
  string(1) "^"
  [35]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(1)
  }
  [36]=>
  string(1) ";"
  [37]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [38]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
    [2]=>
    int(1)
  }
  [39]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [40]=>
  string(1) "="
  [41]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [42]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "4"
    [2]=>
    int(1)
  }
  [43]=>
  string(1) "&"
  [44]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
    [2]=>
    int(1)
  }
  [45]=>
  string(1) ";"
  [46]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [47]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(17) "wrong syntax here"
    [2]=>
    int(1)
  }
}
Done
