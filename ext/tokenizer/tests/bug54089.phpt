--TEST--
Bug #54089 (token_get_all() does not stop after __halt_compiler)
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
$codes = array(
	"<?php __halt_compiler",
	"<?php __halt_compiler(",
	"<?php __halt_compiler();",
	"<?php __halt_compiler();ABC",
	"<?php __halt_compiler\n(\n)\n;ABC",
	"<?php __halt_compiler\nabc\ndef\nghi ABC",
);
foreach ($codes as $code) {
	$tokens = token_get_all($code);
	var_dump($tokens);

	$code = '';
	foreach ($tokens as $t)
	{
		$code .= isset($t[1]) ? $t[1] : $t;
	}
	var_dump($code);
}

?>
--EXPECTF--
array(2) {
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
    string(15) "__halt_compiler"
    [2]=>
    int(1)
  }
}
string(21) "<?php __halt_compiler"
array(3) {
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
    string(15) "__halt_compiler"
    [2]=>
    int(1)
  }
  [2]=>
  string(1) "("
}
string(22) "<?php __halt_compiler("
array(5) {
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
    string(15) "__halt_compiler"
    [2]=>
    int(1)
  }
  [2]=>
  string(1) "("
  [3]=>
  string(1) ")"
  [4]=>
  string(1) ";"
}
string(24) "<?php __halt_compiler();"
array(6) {
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
    string(15) "__halt_compiler"
    [2]=>
    int(1)
  }
  [2]=>
  string(1) "("
  [3]=>
  string(1) ")"
  [4]=>
  string(1) ";"
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "ABC"
    [2]=>
    int(1)
  }
}
string(27) "<?php __halt_compiler();ABC"
array(9) {
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
    string(15) "__halt_compiler"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(1)
  }
  [3]=>
  string(1) "("
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [5]=>
  string(1) ")"
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(3)
  }
  [7]=>
  string(1) ";"
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "ABC"
    [2]=>
    int(4)
  }
}
string(30) "<?php __halt_compiler
(
)
;ABC"
array(9) {
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
    string(15) "__halt_compiler"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(1)
  }
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "abc"
    [2]=>
    int(2)
  }
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(2)
  }
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "def"
    [2]=>
    int(3)
  }
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "
"
    [2]=>
    int(3)
  }
  [7]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "ghi"
    [2]=>
    int(4)
  }
  [8]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) " ABC"
    [2]=>
    int(4)
  }
}
string(37) "<?php __halt_compiler
abc
def
ghi ABC"
