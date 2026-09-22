--TEST--
Bug #79934: CRLF-only line in heredoc causes parsing error
--DESCRIPTION--
This test covers different variations of whitespace-only lines in heredoc strings.
These whitespace-only lines should be ignored when stripping indentation.
--FILE--
<?php
// lines with only CRLF should not cause a parse error
eval("\$s1 = <<<HEREDOC\r\n    a\r\n\r\n    b\r\n    HEREDOC;");
var_dump(addcslashes($s1, "\r\n"));

// lines with only a LF should not cause a parse error
eval("\$s2 = <<<HEREDOC\n    a\n\n    b\n    HEREDOC;");
var_dump(addcslashes($s2, "\n"));

// lines with only a CR should not cause a parse error
eval("\$s3 = <<<HEREDOC\r    a\r\r    b\r    HEREDOC;");
var_dump(addcslashes($s3, "\r"));

// lines with only whitespace should not cause a parse error
eval("\$s4 = <<<HEREDOC\r    a\r\n  \r\n    b\r    HEREDOC;");
var_dump(addcslashes($s4, "\n\r"));

?>
--EXPECT--
string(10) "a\r\n\r\nb"
string(6) "a\n\nb"
string(6) "a\r\rb"
string(10) "a\r\n\r\nb"
