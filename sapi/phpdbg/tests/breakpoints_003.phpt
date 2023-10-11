--TEST--
Test deleting breakpoints
--PHPDBG--
b 4
b del 0
b 5
r
b del 1
r
y
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:4]
prompt> [Deleted breakpoint #0]
prompt> [Breakpoint #1 added at %s:5]
prompt> 12
[Breakpoint #1 at %s:5, hits: 1]
>00005: echo $i++;
 00006: echo $i++;
 00007: 
prompt> [Deleted breakpoint #1]
prompt> Do you really want to restart execution? (type y or n): 1234
[Script ended normally]
prompt> 
--FILE--
<?php
$i = 1;
echo $i++;
echo $i++;
echo $i++;
echo $i++;
?>
