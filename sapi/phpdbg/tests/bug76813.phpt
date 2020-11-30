--TEST--
Bug #76813 (Access_violation_near_NULL_on_source_operand)
--SKIPIF--
<?php
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
}
?>
--PHPDBG--
"#!==)===\377\377\276\242="
#!==)===\377\377\276\242=
--EXPECT--
prompt> [Parse Error: syntax error, unexpected input, expecting $end]
prompt> [Parse Error: syntax error, unexpected # (pound sign), expecting $end]
prompt> [Parse Error: syntax error, unexpected # (pound sign), expecting $end]
prompt>
