--TEST--
defining INI options with -d
--SKIPIF--
<?php 
include "skipif.inc"; 
?>
--FILE--
<?php
include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$file = dirname(__FILE__)."/002.test.php";

file_put_contents($file, '<?php var_dump(ini_get("max_execution_time")); ?>');

var_dump(`$php -n -d max_execution_time=111 $file`);
var_dump(`$php -n -d max_execution_time=500 $file`);
var_dump(`$php -n -d max_execution_time=500 -d max_execution_time=555 $file`);

file_put_contents($file, '<?php var_dump(ini_get("max_execution_time")); var_dump(ini_get("upload_tmp_dir")); ?>');

var_dump(`$php -n -d upload_tmp_dir=/test/path -d max_execution_time=555 $file`);

unlink($file);

echo "Done\n";
?>
--EXPECTF--	
string(%d) "X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

%unicode|string%(3) "111"
"
string(%d) "X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

%unicode|string%(3) "500"
"
string(%d) "X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

%unicode|string%(3) "555"
"
string(%d) "X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

%unicode|string%(3) "555"
%unicode|string%(10) "/test/path"
"
Done
