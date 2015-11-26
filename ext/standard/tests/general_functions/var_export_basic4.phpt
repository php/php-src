--TEST--
Test var_export() function with valid strings
--FILE--
<?php
/* Prototype  : mixed var_export(mixed var [, bool return])
 * Description: Outputs or returns a string representation of a variable 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */


echo "*** Testing var_export() with valid strings ***\n";
// different valid  string 
$valid_strings = array(
            "\"\"" => "",
            "\" \"" => " ",
            "''" => '',
            "' '" => ' ',
            "\"string\"" => "string",
            "'string'" => 'string',
            "\"\\0Hello\\0 World\\0\"" => "\0Hello\0 World\0",
            "\"NULL\"" => "NULL",
            "'null'" => 'null',
            "\"FALSE\"" => "FALSE",
            "'false'" => 'false',
            "\"\\x0b\"" => "\x0b",
            "\"\\0\"" => "\0",
            "'\\0'" => '\0',
            "'\\060'" => '\060',
            "\"\\070\"" => "\070"
);

/* Loop to check for above strings with var_export() */
echo "\n*** Output for strings ***\n";
foreach($valid_strings as $key => $str) {
	echo "\n-- Iteration: $key --\n";
	var_export( $str );
	echo "\n";
	var_export( $str, FALSE);
	echo "\n";
	var_dump( var_export( $str, TRUE) );
	echo "\n";
}

?>
===DONE===
--EXPECT--
*** Testing var_export() with valid strings ***

*** Output for strings ***

-- Iteration: "" --
''
''
string(2) "''"


-- Iteration: " " --
' '
' '
string(3) "' '"


-- Iteration: '' --
''
''
string(2) "''"


-- Iteration: ' ' --
' '
' '
string(3) "' '"


-- Iteration: "string" --
'string'
'string'
string(8) "'string'"


-- Iteration: 'string' --
'string'
'string'
string(8) "'string'"


-- Iteration: "\0Hello\0 World\0" --
'' . "\0" . 'Hello' . "\0" . ' World' . "\0" . ''
'' . "\0" . 'Hello' . "\0" . ' World' . "\0" . ''
string(49) "'' . "\0" . 'Hello' . "\0" . ' World' . "\0" . ''"


-- Iteration: "NULL" --
'NULL'
'NULL'
string(6) "'NULL'"


-- Iteration: 'null' --
'null'
'null'
string(6) "'null'"


-- Iteration: "FALSE" --
'FALSE'
'FALSE'
string(7) "'FALSE'"


-- Iteration: 'false' --
'false'
'false'
string(7) "'false'"


-- Iteration: "\x0b" --
''
''
string(3) "''"


-- Iteration: "\0" --
'' . "\0" . ''
'' . "\0" . ''
string(14) "'' . "\0" . ''"


-- Iteration: '\0' --
'\\0'
'\\0'
string(5) "'\\0'"


-- Iteration: '\060' --
'\\060'
'\\060'
string(7) "'\\060'"


-- Iteration: "\070" --
'8'
'8'
string(3) "'8'"

===DONE===