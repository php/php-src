--TEST--
Bug #81135: unknown help topic fails assertion
--PHPDBG--
help unknown
q
--FILE--
<?php

?>
--EXPECTF--
[Successful compilation of %sbug81135.php]
prompt> [No help topic found for unknown]
prompt>
