--TEST--
GH-23083 (SEGV in build_trace_args() with error_include_args=On and a non-literal include argument)
--INI--
error_include_args=On
--FILE--
<?php

$file = 'no_such_file';

/* CV operand */
include $file;

/* TMP operand */
include $file . '_2';

/* CV operand holding a reference */
$ref = &$file;
include $ref;

?>
--EXPECTF--
Warning: include('no_such_file'): Failed to open stream: No such file or directory in %s on line %d

Warning: include('no_such_file'): Failed opening 'no_such_file' for inclusion (include_path='%s') in %s on line %d

Warning: include('no_such_file_2'): Failed to open stream: No such file or directory in %s on line %d

Warning: include('no_such_file_2'): Failed opening 'no_such_file_2' for inclusion (include_path='%s') in %s on line %d

Warning: include('no_such_file'): Failed to open stream: No such file or directory in %s on line %d

Warning: include('no_such_file'): Failed opening 'no_such_file' for inclusion (include_path='%s') in %s on line %d
