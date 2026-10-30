--TEST--
Bug #61139 (gzopen leaks when specifying invalid mode)
--EXTENSIONS--
zlib
--FILE--
<?php

gzopen('someFile', 'c');
?>
--CLEAN--
<?php
    unlink('someFile');
?>
--EXPECTF--
Warning: gzopen(): gzopen failed in %s on line %d
