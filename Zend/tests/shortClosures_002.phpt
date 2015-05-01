--TEST--
Short closures with parameter being expression must fail
--FILE--
<?php

function parseError($code) {
	try {
		eval($code);
	} catch (ParseError $e) {
		print $code." : ".$e->getMessage()."\n";
	}
}

var_dump((($var) ~> $var + 2)(2));

parseError(<<<'Code'
(($var + 2) ~> $var + 2)(2);
Code
);

parseError(<<<'Code'
(($$var) ~> $var + 2)(2);
Code
);

?>
--EXPECT--
int(4)
(($var + 2) ~> $var + 2)(2); : Cannot use an expression as parameter
(($$var) ~> $var + 2)(2); : Cannot use an expression as parameter

