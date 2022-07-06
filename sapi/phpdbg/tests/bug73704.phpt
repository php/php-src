--TEST--
Bug #73704 (phpdbg shows the wrong line in files with shebang)
--PHPDBG--
list 6
b 4
r
c
q
--EXPECTF--
[Successful compilation of %s]
prompt>  00001: #!/usr/bin/env php
 00002: <?php
 00003: 
 00004: echo 1;
 00005: 
prompt> [Breakpoint #0 added at %s:4]
prompt> [Breakpoint #0 at %s:4, hits: 1]
>00004: echo 1;
 00005: 
prompt> 1
[Script ended normally]
prompt> 
--FILE--
#!/usr/bin/env php
<?php

echo 1;
