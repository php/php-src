--TEST--
GH-12675 (MEMORY_LEAK in phpdbg_prompt.c)
--PHPDBG--
ev file_put_contents("gh12675_1.tmp", "<?php echo 'hi';\necho 2;")
ev file_put_contents("gh12675_2.tmp", "<?php echo 'hi';")
exec nonexistent.php
exec gh12675_1.tmp
exec gh12675_1.tmp
b gh12675_1.tmp:2
r
exec gh12675_2.tmp
n
q
--EXPECTF--
prompt> 24
prompt> 16
prompt> [Cannot stat nonexistent.php, ensure the file exists]
prompt> [Set execution context: %sgh12675_1.tmp]
[Successful compilation of %sgh12675_1.tmp]
prompt> [Execution context not changed]
prompt> [Breakpoint #0 added at %sgh12675_1.tmp:2]
prompt> hi
[Breakpoint #0 at %sgh12675_1.tmp:2, hits: 1]
>00002: echo 2;
prompt> Do you really want to stop execution to set a new execution context? (type y or n): prompt>
--CLEAN--
<?php
@unlink("gh12675_1.tmp");
@unlink("gh12675_2.tmp");
?>
