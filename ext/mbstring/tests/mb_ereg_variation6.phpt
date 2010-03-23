--TEST--
Test mb_ereg() function : usage variations - match special characters
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_ereg(string $pattern, string $string [, array $registers])
 * Description: Regular expression match for multibyte string 
 * Source code: ext/mbstring/php_mbregex.c
 */

/*
 * Test how mb_ereg() matches special characters for $pattern
 */

echo "*** Testing mb_ereg() : usage variations ***\n";

if(mb_regex_encoding('utf-8') == true) {
	echo "Regex encoding set to utf-8\n";
} else {
	echo "Could not set regex encoding to utf-8\n";
}

$regex_char = array ('\w+' => b'\w+', 
                     '\W+' => b'\W+', 
                     '\s+' => b'\s+', 
                     '\S+' => b'\S+', 
                     '\d+' => b'\d+', 
                     '\D+' => b'\D+', 
                     '\b' =>  b'\b', 
                     '\B' =>  b'\B');

$string_ascii = b'This is an English string. 0123456789.';
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

foreach ($regex_char as $displayChar => $char) {
	echo "\n--** Pattern is: $displayChar **--\n";
	if (@$regs_ascii || @$regs_mb) {
		$regs_ascii = null;
		$regs_mb = null;
	}
	echo "-- ASCII String: --\n";
	var_dump(mb_ereg($char, $string_ascii, $regs_ascii));
	base64_encode_var_dump($regs_ascii);

	echo "-- Multibyte String: --\n";
	var_dump(mb_ereg($char, $string_mb, $regs_mb));
	base64_encode_var_dump($regs_mb);

}

/**
 * replicate a var dump of an array but outputted string values are base64 encoded
 *
 * @param array $regs
 */
function base64_encode_var_dump($regs) {
	if ($regs) {
		echo "array(" . count($regs) . ") {\n";
		foreach ($regs as $key => $value) {
			echo "  [$key]=>\n  ";
			if (is_string($value)) {
				var_dump(base64_encode($value));
			} else {
				var_dump($value);
			}
		}
		echo "}\n";
	} else {
		echo "NULL\n";
	}
}

echo "Done";

?>
--EXPECT--
*** Testing mb_ereg() : usage variations ***
Regex encoding set to utf-8

--** Pattern is: \w+ **--
-- ASCII String: --
int(4)
array(1) {
  [0]=>
  string(8) "VGhpcw=="
}
-- Multibyte String: --
int(27)
array(1) {
  [0]=>
  string(36) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ"
}

--** Pattern is: \W+ **--
-- ASCII String: --
int(1)
array(1) {
  [0]=>
  string(4) "IA=="
}
-- Multibyte String: --
int(3)
array(1) {
  [0]=>
  string(4) "44CC"
}

--** Pattern is: \s+ **--
-- ASCII String: --
int(1)
array(1) {
  [0]=>
  string(4) "IA=="
}
-- Multibyte String: --
bool(false)
NULL

--** Pattern is: \S+ **--
-- ASCII String: --
int(4)
array(1) {
  [0]=>
  string(8) "VGhpcw=="
}
-- Multibyte String: --
int(53)
array(1) {
  [0]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
}

--** Pattern is: \d+ **--
-- ASCII String: --
int(10)
array(1) {
  [0]=>
  string(16) "MDEyMzQ1Njc4OQ=="
}
-- Multibyte String: --
int(20)
array(1) {
  [0]=>
  string(28) "MDEyMzTvvJXvvJbvvJfvvJjvvJk="
}

--** Pattern is: \D+ **--
-- ASCII String: --
int(27)
array(1) {
  [0]=>
  string(36) "VGhpcyBpcyBhbiBFbmdsaXNoIHN0cmluZy4g"
}
-- Multibyte String: --
int(30)
array(1) {
  [0]=>
  string(40) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC"
}

--** Pattern is: \b **--
-- ASCII String: --
int(1)
array(1) {
  [0]=>
  bool(false)
}
-- Multibyte String: --
int(1)
array(1) {
  [0]=>
  bool(false)
}

--** Pattern is: \B **--
-- ASCII String: --
int(1)
array(1) {
  [0]=>
  bool(false)
}
-- Multibyte String: --
int(1)
array(1) {
  [0]=>
  bool(false)
}
Done