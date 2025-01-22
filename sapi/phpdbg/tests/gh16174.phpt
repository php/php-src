--TEST--
GH-16174 (Empty string is an invalid expression for phpdbg-ev)
--PHPDBG--
ev $e = ""
ev $f = ''
--EXPECT--
prompt> 
prompt> 
prompt> 
prompt>
