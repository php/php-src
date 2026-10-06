--TEST--
CGI: user.ini is loaded when the script directory prefixes doc_root
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$workDir = __DIR__ . '/user-ini-prefix';
@mkdir($workDir);
file_put_contents($workDir . '/info.php', "<?php echo ini_get('memory_limit'), \"\\n\"; ");
file_put_contents($workDir . '/.user.ini', "memory_limit=77M\n");

$f = $workDir . '/info.php';
putenv("REDIRECT_STATUS=1");
putenv("SCRIPT_FILENAME=" . $f);
putenv("PATH_TRANSLATED=" . $f);
putenv("DOCUMENT_ROOT=" . $workDir . '/www');
putenv("REQUEST_METHOD=GET");
putenv("QUERY_STRING=");

echo `$php -n -d user_ini.cache_ttl=0 $f`;

echo "Done\n";
?>
--EXPECTF--
X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

77M
Done
--CLEAN--
<?php
@unlink(__DIR__ . '/user-ini-prefix/info.php');
@unlink(__DIR__ . '/user-ini-prefix/.user.ini');
@rmdir(__DIR__ . '/user-ini-prefix');
?>
