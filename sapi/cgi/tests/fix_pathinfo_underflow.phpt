--TEST--
cgi fix_pathinfo with PATH_INFO shorter than the stripped suffix
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$d = __DIR__ . '/fix_pathinfo_underflow';
@mkdir($d);
$f = $d . '/info.php';
file_put_contents($f, '<?php echo "PI=[", $_SERVER["PATH_INFO"] ?? "unset", "]", "\n"; ');

putenv("REDIRECT_STATUS=1");
putenv("SCRIPT_FILENAME=" . $f . "/" . str_repeat("a", 300));
putenv("PATH_INFO=/");
putenv("SCRIPT_NAME=/info.php");
putenv("REQUEST_METHOD=GET");
putenv("QUERY_STRING=");

echo `$php -n -d cgi.fix_pathinfo=1 $f`;

echo "Done\n";

@unlink($f);
@rmdir($d);
?>
--EXPECTF--
X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

PI=[/]
Done
