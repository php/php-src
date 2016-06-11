--TEST--
Ensure proper saving of EX(opline)
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> caught Generator exception
[Script ended normally]
prompt> 
--FILE--
<?php

function gen() {
	try {
		throw new Exception;
	} catch(Exception $e) {
		yield "caught Generator exception";
	}
}

foreach (gen() as $v) {
	print $v;
}
