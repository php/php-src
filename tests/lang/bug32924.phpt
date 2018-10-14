--TEST--
Bug #32924 (prepend does not add file to included files)
--INI--
include_path={PWD}
auto_prepend_file=inc.inc
--FILE--
<?php
include_once(dirname(__FILE__).'/inc.inc');
require_once(dirname(__FILE__).'/inc.inc');
?>
END
--EXPECT--
Included!
END
