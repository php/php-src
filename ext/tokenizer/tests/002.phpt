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
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "<?"
  }
  [1]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [2]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "echo"
  }
  [3]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [4]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
  }
  [5]=>
  string(1) ";"
  [6]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [7]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "if"
  }
  [8]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [9]=>
  string(1) "("
  [10]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "isset"
  }
  [11]=>
  string(1) "("
  [12]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [13]=>
  string(1) ")"
  [14]=>
  string(1) ")"
  [15]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [16]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "print"
  }
  [17]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [18]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [19]=>
  string(1) "+"
  [20]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
  }
  [21]=>
  string(1) ";"
  [22]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [23]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [24]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "++"
  }
  [25]=>
  string(1) ";"
  [26]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [27]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [28]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "--"
  }
  [29]=>
  string(1) ";"
  [30]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [31]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [32]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [33]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "=="
  }
  [34]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [35]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
  }
  [36]=>
  string(1) ";"
  [37]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [38]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [39]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [40]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "==="
  }
  [41]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [42]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
  }
  [43]=>
  string(1) ";"
  [44]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [45]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "endif"
  }
  [46]=>
  string(1) ";"
  [47]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [48]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
  }
}
array(37) {
  [0]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<?php "
  }
  [1]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "switch"
  }
  [2]=>
  string(1) "("
  [3]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [4]=>
  string(1) ")"
  [5]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [6]=>
  string(1) "{"
  [7]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [8]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "case"
  }
  [9]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [10]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
  }
  [11]=>
  string(1) ":"
  [12]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [13]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "break"
  }
  [14]=>
  string(1) ";"
  [15]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [16]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(7) "default"
  }
  [17]=>
  string(1) ":"
  [18]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [19]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "break"
  }
  [20]=>
  string(1) ";"
  [21]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [22]=>
  string(1) "}"
  [23]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [24]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "while"
  }
  [25]=>
  string(1) "("
  [26]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [27]=>
  string(1) ")"
  [28]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [29]=>
  string(1) "{"
  [30]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [31]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "exit"
  }
  [32]=>
  string(1) ";"
  [33]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [34]=>
  string(1) "}"
  [35]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [36]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
  }
}
array(48) {
  [0]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "<?"
  }
  [1]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [2]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(13) "/* comment */"
  }
  [3]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [4]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "if"
  }
  [5]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [6]=>
  string(1) "("
  [7]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
  }
  [8]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [9]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "||"
  }
  [10]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [11]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
  }
  [12]=>
  string(1) ")"
  [13]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [14]=>
  string(1) "{"
  [15]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [16]=>
  string(1) "}"
  [17]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [18]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$a"
  }
  [19]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [20]=>
  string(1) "="
  [21]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [22]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
  }
  [23]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [24]=>
  string(1) "|"
  [25]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [26]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
  }
  [27]=>
  string(1) ";"
  [28]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [29]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$b"
  }
  [30]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [31]=>
  string(1) "="
  [32]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [33]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "3"
  }
  [34]=>
  string(1) "^"
  [35]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
  }
  [36]=>
  string(1) ";"
  [37]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [38]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "$c"
  }
  [39]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [40]=>
  string(1) "="
  [41]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [42]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "4"
  }
  [43]=>
  string(1) "&"
  [44]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "2"
  }
  [45]=>
  string(1) ";"
  [46]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(1) " "
  }
  [47]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "?>"
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(%d)
    [1]=>
    string(17) "wrong syntax here"
  }
}
Done
