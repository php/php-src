--TEST--
Bug #75574 putenv does not work properly if parameter contains non-ASCII unicode character, UTF-8
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
/*
#vim: set fileencoding=utf-8
#vim: set encoding=utf-8
*/

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$fn = __DIR__ . DIRECTORY_SEPARATOR . md5(uniqid());
file_put_contents($fn, "<?php\nvar_dump(putenv('FOO=啊'));\n//var_dump(`echo %FOO%`);\nvar_dump(getenv('FOO'));");

echo shell_exec("$php -n -f $fn");

unlink($fn);

?>
--EXPECT--
bool(true)
string(3) "啊"
