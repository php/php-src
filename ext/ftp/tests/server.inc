<?php

$socket = null;
$errno = 0;
$context = stream_context_create(array('ssl' => array('local_cert' => dirname(__FILE__).'/cert.pem', 'passphrase' => 'pass')));

for ($i=0; $i<10 && !$socket; ++$i) {
	$port = rand(50000, 65535);
	$socket = stream_socket_server("tcp://127.0.0.1:$port", $errno, $errstr, STREAM_SERVER_BIND|STREAM_SERVER_LISTEN, $context);
}

if (!$socket) {
	die("could not start/bind the ftp server\n");
}

$pid = pcntl_fork();

if ($pid) {

function dump_and_exit($buf)
{
	var_dump($buf);
	fclose($GLOBALS['s']);
	exit;
}

function anonymous()
{
	return $GLOBALS['user'] === 'anonymous';
}

/* quick&dirty realpath() like function */
function change_dir($dir)
{
	global $cwd;

	if ($dir[0] == '/') {
		$cwd = $dir;
		return;
	}

	$cwd = "$cwd/$dir";

	do {
		$old = $cwd;
		$cwd = preg_replace('@/?[^/]+/\.\.@', '', $cwd);
	} while ($old != $cwd);

	$cwd = strtr($cwd, array('//' => '/'));
	if (!$cwd) $cwd = '/';
}


$s = stream_socket_accept($socket);
if (!$s) die("Error accepting a new connection\n");

fputs($s, "220----- PHP FTP server 0.3 -----\r\n220 Service ready\r\n");
$buf = fread($s, 2048);


function user_auth($buf) {
	global $user, $s, $ssl;

if (!empty($ssl)) {
	if ($buf !== "AUTH TLS\r\n") {
		fputs($s, "500 Syntax error, command unrecognized.\r\n");
		dump_and_exit($buf);
	}

	fputs($s, "234 auth type accepted\r\n");

	if (!stream_socket_enable_crypto($s, true, STREAM_CRYPTO_METHOD_SSLv23_SERVER)) {
		die("SSLv23 handshake failed.\n");
	}

	if (!preg_match('/^PBSZ \d+\r\n$/', $buf = fread($s, 2048))) {
		fputs($s, "501 bogus data\r\n");
		dump_and_exit($buf);
	}

	fputs($s, "200 OK\r\n");
	$buf = fread($s, 2048);

	if ($buf !== "PROT P\r\n") {
		fputs($s, "504 Wrong protection.\r\n");
		dump_and_exit($buf);
	}

	fputs($s, "200 OK\r\n");

	$buf = fread($s, 2048);
}

if (!preg_match('/^USER (\w+)\r\n$/', $buf, $m)) {
	fputs($s, "500 Syntax error, command unrecognized.\r\n");
	dump_and_exit($buf);
}
$user = $m[1];
if ($user !== 'user' && $user !== 'anonymous') {
	fputs($s, "530 Not logged in.\r\n");
	fclose($s);
	exit;
}

if (anonymous()) {
	fputs($s, "230 Anonymous user logged in\r\n");

} else {
	fputs($s, "331 User name ok, need password\r\n");

	if (!preg_match('/^PASS (\w+)\r\n$/', $buf = fread($s, 100), $m)) {
		fputs($s, "500 Syntax error, command unrecognized.\r\n");
		dump_and_exit($buf);
	}

	$pass = $m[1];
	if ($pass === 'pass') {
		fputs($s, "230 User logged in\r\n");
	} else {
		fputs($s, "530 Not logged in.\r\n");
		fclose($s);
		exit;
	}
}
}

user_auth($buf);

$cwd = '/';
$num_bogus_cmds = 0;

while($buf = fread($s, 4098)) {

	if (!empty($bogus)) {
		fputs($s, "502 Command not implemented (".$num_bogus_cmds++.").\r\n");

	} else if ($buf === "HELP\r\n") {
		fputs($s, "214-There is help available for the following commands:\r\n");
		fputs($s, " USER\r\n");
		fputs($s, " HELP\r\n");
		fputs($s, "214 end of list\r\n");

	} elseif ($buf === "HELP HELP\r\n") {
		fputs($s, "214 Syntax: HELP [<SP> <string>] <CRLF>\r\n");

	} elseif ($buf === "PWD\r\n") {
		fputs($s, "257 \"$cwd\" is current directory.\r\n");

	} elseif ($buf === "CDUP\r\n") {
		change_dir('..');
		fputs($s, "250 CDUP command successful.\r\n");

	} elseif ($buf === "SYST\r\n") {
		if (isset($bug27809)) {
			fputs($s, "215   OS/400 is the remote operating system. The TCP/IP version is \"V5R2M0\"\r\n");
		} else {
			fputs($s, "215 UNIX Type: L8.\r\n");
		}

	} elseif ($buf === "QUIT\r\n") {
		break;

	} elseif (preg_match("~^CWD ([A-Za-z./]+)\r\n$~", $buf, $m)) {
		change_dir($m[1]);
		fputs($s, "250 CWD command successful.\r\n");

	} elseif (preg_match("~^MKD ([A-Za-z./]+)\r\n$~", $buf, $m)) {
		if (isset($bug7216)) {
			fputs($s, "257 OK.\r\n");
		} else {
			fputs($s, '257 "'.realpath($m[1])."\" created.\r\n");
		}

	} elseif (preg_match('/^USER /', $buf)) {
		user_auth($buf);

	} else {
		fputs($s, "500 Syntax error, command unrecognized.\r\n");
		dump_and_exit($buf);
	}
}

fclose($s);
exit;
}

fclose($socket);
?>
