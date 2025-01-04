--TEST--
Verify that tokenization of file contents includes attributes on constants
--EXTENSIONS--
tokenizer
--FILE--
<?php

#[MyAttribute("foo", 123, bar: "baz", qux: true)]
const EXAMPLE = 'Value';

$tokens = PhpToken::tokenize( file_get_contents( __FILE__ ) );
$relevant = array_slice( $tokens, 0, 32 );

foreach ($relevant as $token) {
    echo "Line {$token->line}: {$token->getTokenName()} ('{$token->text}')\n";
}

?>
--EXPECT--
Line 1: T_OPEN_TAG ('<?php
')
Line 2: T_WHITESPACE ('
')
Line 3: T_ATTRIBUTE ('#[')
Line 3: T_STRING ('MyAttribute')
Line 3: ( ('(')
Line 3: T_CONSTANT_ENCAPSED_STRING ('"foo"')
Line 3: , (',')
Line 3: T_WHITESPACE (' ')
Line 3: T_LNUMBER ('123')
Line 3: , (',')
Line 3: T_WHITESPACE (' ')
Line 3: T_STRING ('bar')
Line 3: : (':')
Line 3: T_WHITESPACE (' ')
Line 3: T_CONSTANT_ENCAPSED_STRING ('"baz"')
Line 3: , (',')
Line 3: T_WHITESPACE (' ')
Line 3: T_STRING ('qux')
Line 3: : (':')
Line 3: T_WHITESPACE (' ')
Line 3: T_STRING ('true')
Line 3: ) (')')
Line 3: ] (']')
Line 3: T_WHITESPACE ('
')
Line 4: T_CONST ('const')
Line 4: T_WHITESPACE (' ')
Line 4: T_STRING ('EXAMPLE')
Line 4: T_WHITESPACE (' ')
Line 4: = ('=')
Line 4: T_WHITESPACE (' ')
Line 4: T_CONSTANT_ENCAPSED_STRING (''Value'')
Line 4: ; (';')
