--TEST--
Parse exceptions when using require
--INI--
allow_url_include=1
--FILE--
<?php


function test_parse_error($code) {
    try {
        require 'data://text/plain;base64,' . base64_encode($code);
    } catch (ParseException $e) {
        echo $e->getMessage(), "\n";
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

?>
--EXPECT--
syntax error, unexpected end of file
syntax error, unexpected end of file
syntax error, unexpected end of file, expecting '('
