--TEST--
yield from is tokenized separately
--FILE--
<?php

function tokens($str) {
	$t = token_get_all("<?php $str");
	array_shift($t);
	foreach ($t as $t) {
		echo is_array($t) ? $t[1] : $t, "|";
	}
	print "\n";
}

tokens("yield from \$foo;");
tokens("yield # comment\n/* other comment */ from \$foo;");

?>
--EXPECT--
yield| |from| |$foo|;|
yield| |# comment|
|/* other comment */| |from| |$foo|;|
