--TEST--
GH-10634 (Lexing memory corruption)
--FILE--
<?php
function test_input($input) {
    try {
        eval($input);
    } catch(Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

test_input("y&/*");
test_input("y&/**");
test_input("y&#");
test_input("y&#  ");
test_input("y&//");
?>
--EXPECT--
ParseError: Unterminated comment starting line 1
ParseError: Unterminated comment starting line 1
ParseError: syntax error, unexpected end of file
ParseError: syntax error, unexpected end of file
ParseError: syntax error, unexpected end of file
