--TEST--
Built-in web server with root as the docroot
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";

if (PHP_OS_FAMILY === 'Windows') {
	$doc_root = '\\';
} else {
	$doc_root = '/';
}

$server = php_cli_server_start(null, null, [], $doc_root);
assert($server->docRoot === $doc_root);

file_put_contents(__DIR__ . '/public.txt', 'Public file contents');

$tmpFile = tempnam(sys_get_temp_dir(), "php_cli_server_025");
@unlink($tmpFile);
mkdir($tmpFile);
file_put_contents($tmpFile . '/secret-config.txt', 'Secret credentials, NOT PUBLIC');

symlink($tmpFile, __DIR__ . '/escape');
symlink($tmpFile . '/secret-config.txt', __DIR__ . '/leak.txt');

$targets = [
	__DIR__ . "/public.txt",
	__DIR__ . "/../traversal.txt",
	__DIR__ . "/leak.txt",
	__DIR__ . "/escape/secret-config.txt",
];
foreach ( $targets as $target ) {
	$fp = php_cli_server_connect();
	$req = <<<END
GET $target HTTP/1.1



END;

	echo "$target:\n";
	if (fwrite($fp, $req)) {
		$had_response = false;
		while (!feof($fp)) {
			$had_response = true;
			echo fgets($fp);
		}
		if (!$had_response) {
			echo "No response from server?\n";
		}
	} else {
		echo "fwrite() failed?\n";
	}
	echo "\n\n----------\n";
	fclose($fp);
}

unlink(__DIR__ . '/public.txt');
unlink(__DIR__ . '/leak.txt');
// On Windows, to delete a symlink to a directory, rmdir() has to be used instead.
if (PHP_OS_FAMILY === 'Windows') {
	rmdir(__DIR__ . '/escape');
} else {
	unlink(__DIR__ . '/escape');
}

?>
--EXPECTF--
%s/public.txt:
HTTP/1.1 200 OK
Date: %s
Connection: close
Content-Type: text/plain; charset=UTF-8
Content-Length: %d

Public file contents

----------
%s/../traversal.txt:
HTTP/1.1 404 Not Found
Date: %s
Connection: close
X-Powered-By: %s
Content-Type: text/html; charset=UTF-8
Content-Length: %d

<!doctype html><html><head><meta name="viewport" content="width=device-width, initial-scale=1"><title>404 Not Found</title><style>
body { background-color: #fcfcfc; color: #333333; margin: 0; padding:0; }
h1 { font-size: 1.5em; font-weight: normal; background-color: #9999cc; min-height:2em; line-height:2em; border-bottom: 1px inset black; margin: 0; }
h1, p { padding-left: 10px; }
code.url { background-color: #eeeeee; font-family:monospace; padding:0 2px;}
</style>
</head><body><h1>Not Found</h1><p>The requested resource <code class="url">%s/../traversal.txt</code> was not found on this server.</p></body></html>

----------
%s/leak.txt:
HTTP/1.1 200 OK
Date: %s
Connection: close
Content-Type: text/plain; charset=UTF-8
Content-Length: %d

Secret credentials, NOT PUBLIC

----------
%s/escape/secret-config.txt:
HTTP/1.1 200 OK
Date: %s
Connection: close
Content-Type: text/plain; charset=UTF-8
Content-Length: %d

Secret credentials, NOT PUBLIC

----------
