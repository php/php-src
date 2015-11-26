--TEST--
Reconstructing a script using token_get_all()
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php

$phpstr = '
<?php

// A php script to test token_get_all()

/* a different
type of
comment */

// a class
class TestClass {
	public function foo() {
		echo "Called foo()\n";
	}
}

$a = new TestClass();
$a->foo();

for ($i = 0; $i < 10; $i++) {
	echo "Loop iteration $i\n";
}

?>';

$token_array = token_get_all($phpstr);

$script = "";
// reconstruct a script (without open/close tags) from the token array 
foreach ($token_array as $token) {
	if (is_array($token)) {
		if (strncmp($token[1], '<?php', 5) == 0) {
			continue;
		}
		if (strncmp($token[1], '?>', 2) == 0) {
			continue;
		}
		$script .= $token[1];
	} else {
		$script .= $token;
	}
}

var_dump($script);

eval($script);

?>
--EXPECT--
string(259) "

// A php script to test token_get_all()

/* a different
type of
comment */

// a class
class TestClass {
	public function foo() {
		echo "Called foo()\n";
	}
}

$a = new TestClass();
$a->foo();

for ($i = 0; $i < 10; $i++) {
	echo "Loop iteration $i\n";
}

"
Called foo()
Loop iteration 0
Loop iteration 1
Loop iteration 2
Loop iteration 3
Loop iteration 4
Loop iteration 5
Loop iteration 6
Loop iteration 7
Loop iteration 8
Loop iteration 9
