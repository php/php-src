--TEST--
Bug #32924 (prepend does not add file to included files)
--INI--
include_path=.
auto_prepend_file=tests/lang/inc.inc
--FILE--
<?php 
include_once('tests/lang/inc.inc');
require_once('tests/lang/inc.inc');
?>
END
--EXPECT--
Included!
END
