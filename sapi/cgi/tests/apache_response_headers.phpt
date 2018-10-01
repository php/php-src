--TEST--
apache_response_headers()
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$filename = dirname(__FILE__) . DIRECTORY_SEPARATOR ."apache_response_headers.test.php";

$code  = '<?php';
$code .= '
header( "X-Robots-Tag : noindex,nofollow,noarchive" );
header( "Content-type: text/html; charset=UTF-8" );
header( "Bad-header" );
header( " : " );
header( ":" );
flush();
foreach ( apache_response_headers() as $hdr_key => $hdr_val ) {
        echo "$hdr_key => $hdr_val\n";
}
?>
';

file_put_contents($filename, $code);

passthru("$php -n -q $filename");

@unlink($filename);

?>
===DONE===
--EXPECTF--
X-Powered-By => PHP/%s
X-Robots-Tag => noindex,nofollow,noarchive
Content-type => text/html; charset=UTF-8
===DONE===
