--TEST--
GH-10634 (Lexing memory corruption)
--FILE--
<?php
function test_input($input) {
    try {
        eval($input);
    } catch(Throwable $e) {
        var_dump($e->getMessage());
    }
}

test_input("y&/*");
test_input("y&/**");
test_input("y&#");
test_input("y&#  ");
test_input("y&//");
?>
--EXPECT--
string(36) "Unterminated comment starting line 1"
string(36) "Unterminated comment starting line 1"
string(36) "syntax error, unexpected end of file"
string(36) "syntax error, unexpected end of file"
string(36) "syntax error, unexpected end of file"
