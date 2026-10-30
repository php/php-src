--TEST--
GH-17387 (Trivial crash in phpdbg lexer)
--PHPDBG--
a';
q
--EXPECT--
prompt> [The command "a" could not be found]
prompt> 
