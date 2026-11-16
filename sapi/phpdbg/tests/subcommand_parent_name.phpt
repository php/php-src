--TEST--
Subcommand error messages name the correct parent command
--PHPDBG--
list lines
print func
break at
info literal 1
set prompt 1
q
--EXPECT--
prompt> [The command "list lines" expected at least 1 arguments (l) and received 0]
prompt> [The command "print func" expected string and got nothing at parameter 1]
prompt> [The command "break at" expected at least 2 arguments (*c) and received 0]
prompt> [The command "info literal" expected no arguments]
prompt> [The command "set prompt" expected string and got numeric at parameter 1]
prompt>
