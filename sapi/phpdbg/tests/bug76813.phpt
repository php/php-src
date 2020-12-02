--TEST--
Bug #76813 (Access_violation_near_NULL_on_source_operand)
--PHPDBG--
"#!==)===\377\377\276\242="
#!==)===\377\377\276\242=
--EXPECT--
prompt> [Parse Error: syntax error, unexpected input, expecting end of command]
prompt> [Parse Error: syntax error, unexpected # (pound sign), expecting end of command]
prompt> [Parse Error: syntax error, unexpected # (pound sign), expecting end of command]
prompt>
