--TEST--
Bug #67741 (auto_prepend_file messes up __LINE__)
--INI--
include_path={PWD}
auto_prepend_file=bug67741_stub.inc
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
#!/bin/env php
<?php
echo "primary lineno: ", __LINE__, "\n";
?>
--EXPECT--
prepend lineno: 2
primary lineno: 3