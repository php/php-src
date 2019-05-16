--TEST--
trim(), rtrim() and ltrim() functions
--FILE--
<?php

$tests = <<<TESTS
'ABC' ===  trim('ABC')
'ABC' === ltrim('ABC')
'ABC' === rtrim('ABC')
'ABC' ===  trim(" \\0\\t\\nABC \\0\\t\\n")
"ABC \\0\\t\\n" === ltrim(" \\0\\t\\nABC \\0\\t\\n")
" \\0\\t\\nABC" === rtrim(" \\0\\t\\nABC \\0\\t\\n")
" \\0\\t\\nABC \\0\\t\\n" ===  trim(" \\0\\t\\nABC \\0\\t\\n",'')
" \\0\\t\\nABC \\0\\t\\n" === ltrim(" \\0\\t\\nABC \\0\\t\\n",'')
" \\0\\t\\nABC \\0\\t\\n" === rtrim(" \\0\\t\\nABC \\0\\t\\n",'')
"ABC\\x50\\xC1" === trim("ABC\\x50\\xC1\\x60\\x90","\\x51..\\xC0")
"ABC\\x50" === trim("ABC\\x50\\xC1\\x60\\x90","\\x51..\\xC1")
"ABC" === trim("ABC\\x50\\xC1\\x60\\x90","\\x50..\\xC1")
"ABC\\x50\\xC1" === trim("ABC\\x50\\xC1\\x60\\x90","\\x51..\\xC0")
"ABC\\x50" === trim("ABC\\x50\\xC1\\x60\\x90","\\x51..\\xC1")
"ABC" === trim("ABC\\x50\\xC1\\x60\\x90","\\x50..\\xC1")
TESTS;

include(__DIR__ . '/../../../../tests/quicktester.inc');
--EXPECT--
OK
