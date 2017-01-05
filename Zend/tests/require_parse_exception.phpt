--TEST--
Parse exceptions when using require
--INI--
allow_url_include=1
--FILE--
<?php

function test_parse_error($code) {
    try {
        require 'data://text/plain;base64,' . base64_encode($code);
    } catch (ParseError $e) {
        echo $e->getMessage(), " on line ", $e->getLine(), "\n";
    }
}

test_parse_error(<<<'EOC'
<?php
{ { { { { }
EOC
);

test_parse_error(<<<'EOC'
<?php
/** doc comment */
function f() {
EOC
);

test_parse_error(<<<'EOC'
<?php
empty
EOC
);

test_parse_error('<?php
var_dump(078);');

test_parse_error('<?php
var_dump("\u{xyz}");');
test_parse_error('<?php
var_dump("\u{ffffff}");');

?>
--EXPECT--
syntax error, unexpected end of file on line 2
syntax error, unexpected end of file on line 3
syntax error, unexpected end of file, expecting '(' on line 2
Invalid numeric literal on line 2
Invalid UTF-8 codepoint escape sequence on line 2
Invalid UTF-8 codepoint escape sequence: Codepoint too large on line 2
