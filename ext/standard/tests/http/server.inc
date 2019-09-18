<?php

function http_server_skipif($socket_string) {

	if (!function_exists('pcntl_fork')) die('skip pcntl_fork() not available');
	if (!function_exists('posix_kill')) die('skip posix_kill() not available');
	if (!stream_socket_server($socket_string)) die('skip stream_socket_server() failed');
}

function http_server_init($socket_string, &$output = null) {
	pcntl_alarm(60);

	$server = stream_socket_server($socket_string, $errno, $errstr);
	if (!$server) {
		return false;
	}

	if ($output === null) {
		$output = tmpfile();
		if ($output === false) {
			return false;
		}
	}

	$pid = pcntl_fork();
	if ($pid == -1) {
		die('could not fork');
	} else if ($pid) {
		return $pid;
	}

	return $server;
}

/* Minimal HTTP server with predefined responses.
 *
 * $socket_string is the socket to create and listen on (e.g. tcp://127.0.0.1:1234)
 * $files is an array of files containing N responses for N expected requests. Server dies after N requests.
 * $output is a stream on which everything sent by clients is written to
 */
function http_server($socket_string, array $files, &$output = null) {

	if (!is_resource($server = http_server_init($socket_string, $output))) {
		return $server;
	}

	foreach($files as $file) {

		$sock = stream_socket_accept($server);
		if (!$sock) {
			exit(1);
		}

		// read headers

		$content_length = 0;

		stream_set_blocking($sock, 0);
		while (!feof($sock)) {

			list($r, $w, $e) = array(array($sock), null, null);
			if (!stream_select($r, $w, $e, 1)) continue;

			$line = stream_get_line($sock, 8192, "\r\n");
			if ($line === '') {
				fwrite($output, "\r\n");
				break;
			} else if ($line !== false) {
				fwrite($output, "$line\r\n");

				if (preg_match('#^Content-Length\s*:\s*([[:digit:]]+)\s*$#i', $line, $matches)) {
					$content_length = (int) $matches[1];
				}
			}
		}
		stream_set_blocking($sock, 1);

		// read content

		if ($content_length > 0) {
			stream_copy_to_stream($sock, $output, $content_length);
		}

		// send response

		$fd = fopen($file, 'rb');
		stream_copy_to_stream($fd, $sock);

		fclose($sock);
	}

	exit(0);
}

function http_server_sleep($socket_string, $micro_seconds = 500000)
{
	if (!is_resource($server = http_server_init($socket_string, $output))) {
		return $server;
	}

	$sock = stream_socket_accept($server);
	if (!$sock) {
		exit(1);
	}

	usleep($micro_seconds);

	fclose($sock);

	exit(0);
}

function http_server_kill($pid) {
	posix_kill($pid, SIGTERM);
	pcntl_waitpid($pid, $status);
}

?>
