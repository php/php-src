--TEST--
Bug #71273 A wrong ext directory setup in php.ini leads to crash
--FILE--
<?php
    /* NOTE this file is required to be encoded in iso-8859-1 */

    $cmd = getenv('TEST_PHP_EXECUTABLE_ESCAPED') . " -n -d html_errors=on -d extension_dir=a/é/w -d extension=php_kartoffelbrei.dll -v 2>&1";
    $out = shell_exec($cmd);

    var_dump(preg_match(",.+a[\\/].+[\\/]w.php_kartoffelbrei.dll.+,s", $out));
?>
--EXPECT--
int(1)
