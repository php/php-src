--TEST--
Bug #60180 ($_SERVER["PHP_SELF"] incorrect)
--SKIPIF--
<?php
if(!file_exists(dirname(__FILE__)."/../../../../sapi/cli/tests/php_cli_server.inc")) 
	die("skip could not found cli server script");
$res = @include dirname(__FILE__)."/../../../../sapi/cli/tests/php_cli_server.inc";
if(!$res) {
	die("skip could not open cli server script");
}

if (CURL_WRAPPERS_ENABLED) {
	die("skip curl wrappers used");
}
?>
--FILE--
<?php
include dirname(__FILE__)."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start(file_get_contents(dirname(__FILE__).'/bug64433_srv.inc'));

echo file_get_contents("http://".PHP_CLI_SERVER_ADDRESS."/index.php");
echo "default\n";
$codes = array(200, 201, 204, 301, 302, 303, 304, 305, 307, 404, 500);
foreach($codes as $code) {
	echo "$code: ".file_get_contents("http://".PHP_CLI_SERVER_ADDRESS."/index.php?status=$code&loc=1");
}
echo "follow=0\n";
$arr = array('http'=>
                        array(
                                'follow_location'=>0,	
                        )
                );
$context = stream_context_create($arr);
foreach($codes as $code) {
	echo "$code: ".file_get_contents("http://".PHP_CLI_SERVER_ADDRESS."/index.php?status=$code&loc=1", false, $context);
}
echo "follow=1\n";
$arr = array('http'=>
                        array(
                                'follow_location'=>1,	
                        )
                );
$context = stream_context_create($arr);
foreach($codes as $code) {
	echo "$code: ".file_get_contents("http://".PHP_CLI_SERVER_ADDRESS."/index.php?status=$code&loc=1", false, $context);
}
--EXPECT--
HELLO!
default
200: HELLO!
201: HELLO!
204: HELLO!
301: REDIRECTED
302: REDIRECTED
303: REDIRECTED
304: HELLO!
305: HELLO!
307: REDIRECTED
404: HELLO!
500: HELLO!
follow=0
200: HELLO!
201: HELLO!
204: HELLO!
301: HELLO!
302: HELLO!
303: HELLO!
304: HELLO!
305: HELLO!
307: HELLO!
404: HELLO!
500: HELLO!
follow=1
200: REDIRECTED
201: REDIRECTED
204: REDIRECTED
301: REDIRECTED
302: REDIRECTED
303: REDIRECTED
304: REDIRECTED
305: REDIRECTED
307: REDIRECTED
404: REDIRECTED
500: REDIRECTED


