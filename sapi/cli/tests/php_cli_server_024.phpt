--TEST--
Built-in web server docroot limit can be bypassed with symlinks
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";

$server = php_cli_server_start(null, null);
$docRoot = $server->docRoot;

file_put_contents($docRoot . '/public.txt', 'Public file contents');

$tmpFile = tempnam(sys_get_temp_dir(), "php_cli_server_024");
@unlink($tmpFile);
mkdir($tmpFile);
file_put_contents($tmpFile . '/secret-config.txt', 'Secret credentials, NOT PUBLIC');

symlink($tmpFile, $docRoot . '/escape');
symlink($tmpFile . '/secret-config.txt', $docRoot . '/leak.txt');

file_put_contents($docRoot . '-neighbor.txt', 'More secret credentials');
symlink($docRoot . '-neighbor.txt', $docRoot . '/neighbor.txt');

$targets = [
	"/public.txt",
	"/../traversal.txt",
	"/leak.txt",
	"/escape/secret-config.txt",
	"/neighbor.txt",
];
foreach ( $targets as $target ) {
	$fp = php_cli_server_connect();
	$req = <<<END
GET $target HTTP/1.1



END;

	echo "$target:\n";
	if (fwrite($fp, $req)) {
		while (!feof($fp)) {
			echo fgets($fp);
		}
	}
	echo "\n\n----------\n";
	fclose($fp);
}

unlink($docRoot . '/public.txt');
unlink($docRoot . '/leak.txt');
// On Windows, to delete a symlink to a directory, rmdir() has to be used instead.
if (PHP_OS_FAMILY === 'Windows') {
	rmdir($docRoot . '/escape');
} else {
	unlink($docRoot . '/escape');
}
unlink($docRoot . '/neighbor.txt');
unlink($docRoot . '-neighbor.txt');

?>
--EXPECTF--
/public.txt:
HTTP/1.1 200 OK
Date: %s
Connection: close
Content-Type: text/plain; charset=UTF-8
Content-Length: 20

Public file contents

----------
/../traversal.txt:
HTTP/1.1 404 Not Found
Date: %s
Connection: close
X-Powered-By: %s
Content-Type: text/html; charset=UTF-8
Content-Length: 617

<!doctype html><html><head><meta name="viewport" content="width=device-width, initial-scale=1"><title>404 Not Found</title><style>
body { background-color: #fcfcfc; color: #333333; margin: 0; padding:0; }
h1 { font-size: 1.5em; font-weight: normal; background-color: #9999cc; min-height:2em; line-height:2em; border-bottom: 1px inset black; margin: 0; }
h1, p { padding-left: 10px; }
code.url { background-color: #eeeeee; font-family:monospace; padding:0 2px;}
</style>
</head><body><h1>Not Found</h1><p>The requested resource <code class="url">/../traversal.txt</code> was not found on this server.</p></body></html>

----------
/leak.txt:
HTTP/1.1 404 Not Found
Date: %s
Connection: close
X-Powered-By: PHP/8.6.0-dev
Content-Type: text/html; charset=UTF-8
Content-Length: 609

<!doctype html><html><head><meta name="viewport" content="width=device-width, initial-scale=1"><title>404 Not Found</title><style>
body { background-color: #fcfcfc; color: #333333; margin: 0; padding:0; }
h1 { font-size: 1.5em; font-weight: normal; background-color: #9999cc; min-height:2em; line-height:2em; border-bottom: 1px inset black; margin: 0; }
h1, p { padding-left: 10px; }
code.url { background-color: #eeeeee; font-family:monospace; padding:0 2px;}
</style>
</head><body><h1>Not Found</h1><p>The requested resource <code class="url">/leak.txt</code> was not found on this server.</p></body></html>

----------
/escape/secret-config.txt:
HTTP/1.1 404 Not Found
Date: %s
Connection: close
X-Powered-By: PHP/8.6.0-dev
Content-Type: text/html; charset=UTF-8
Content-Length: 625

<!doctype html><html><head><meta name="viewport" content="width=device-width, initial-scale=1"><title>404 Not Found</title><style>
body { background-color: #fcfcfc; color: #333333; margin: 0; padding:0; }
h1 { font-size: 1.5em; font-weight: normal; background-color: #9999cc; min-height:2em; line-height:2em; border-bottom: 1px inset black; margin: 0; }
h1, p { padding-left: 10px; }
code.url { background-color: #eeeeee; font-family:monospace; padding:0 2px;}
</style>
</head><body><h1>Not Found</h1><p>The requested resource <code class="url">/escape/secret-config.txt</code> was not found on this server.</p></body></html>

----------
/neighbor.txt:
HTTP/1.1 404 Not Found
Date: %s
Connection: close
X-Powered-By: PHP/8.6.0-dev
Content-Type: text/html; charset=UTF-8
Content-Length: 613

<!doctype html><html><head><meta name="viewport" content="width=device-width, initial-scale=1"><title>404 Not Found</title><style>
body { background-color: #fcfcfc; color: #333333; margin: 0; padding:0; }
h1 { font-size: 1.5em; font-weight: normal; background-color: #9999cc; min-height:2em; line-height:2em; border-bottom: 1px inset black; margin: 0; }
h1, p { padding-left: 10px; }
code.url { background-color: #eeeeee; font-family:monospace; padding:0 2px;}
</style>
</head><body><h1>Not Found</h1><p>The requested resource <code class="url">/neighbor.txt</code> was not found on this server.</p></body></html>

----------
