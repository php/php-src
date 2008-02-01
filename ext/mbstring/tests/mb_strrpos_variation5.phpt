--TEST--
Test mb_strrpos() function : usage variations - pass encoding as third argument (deprecated behaviour)
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrpos') or die("skip mb_strrpos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strrpos(string $haystack, string $needle [, int $offset [, string $encoding]])
 * Description: Find position of last occurrence of a string within another 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Testing deprecated behaviour where third argument can be $encoding
 */

echo "*** Testing mb_strrpos() : usage variations ***\n";

$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');
$needle_mb = base64_decode('44CC');

$stringh = <<<END
utf-8
END;

$inputs = array('Double Quoted String' => "utf-8",
                'Single Quoted String' => 'utf-8',
                'Heredoc' => $stringh);
foreach ($inputs as $type => $input) {
	echo "\n-- $type --\n";
	echo "-- With fourth encoding argument --\n";
	var_dump(mb_strrpos($string_mb, $needle_mb, $input, 'utf-8'));
	echo "-- Without fourth encoding argument --\n";
	var_dump(mb_strrpos($string_mb, $needle_mb, $input));
}

echo "Done";
?>
--EXPECTF--
*** Testing mb_strrpos() : usage variations ***

-- Double Quoted String --
-- With fourth encoding argument --
int(20)
-- Without fourth encoding argument --
int(20)

-- Single Quoted String --
-- With fourth encoding argument --
int(20)
-- Without fourth encoding argument --
int(20)

-- Heredoc --
-- With fourth encoding argument --
int(20)
-- Without fourth encoding argument --
int(20)
Done