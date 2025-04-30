--TEST--
GH-16630 (UAF in lexer with encoding translation and heredocs)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=On
zend.script_encoding=ISO-8859-1
internal_encoding=EUC-JP
--FILE--
<?php
$data3 = <<<CODE
heredoc
text
CODE;
echo $data3;
?>
--EXPECT--
heredoc
text
