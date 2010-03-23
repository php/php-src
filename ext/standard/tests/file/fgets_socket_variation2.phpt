--TEST--
fgets() over a socket with more than a buffer's worth of data
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php

// create a file
$filename = __FILE__ . ".tmp";
$fd = fopen($filename, "w+");

// populate the file with lines of data
define("LINE_OF_DATA", "12345678\n");
for ($i = 0; $i < 1000; $i++) {
	fwrite($fd, LINE_OF_DATA);
}
fclose($fd);

/* Setup socket server */
$server = stream_socket_server('tcp://127.0.0.1:31337');

/* Connect to it */
$client = fsockopen('tcp://127.0.0.1:31337');

if (!$client) {
	die("Unable to create socket");
}

/* Accept that connection */
$socket = stream_socket_accept($server);

echo "Write data from the file:\n";
$data = file_get_contents($filename);
unlink($filename);

var_dump(fwrite($socket, $data));
fclose($socket);

echo "\nRead lines from the client\n";
while ($line = fgets($client,256)) {
	if (strcmp($line, LINE_OF_DATA) != 0) {
		echo "Error - $line does not match " . LINE_OF_DATA;
		break;
	}
}

echo "\nClose the server side socket and read the remaining data from the client\n";
fclose($server);
while(!feof($client)) {
	fread($client, 1);
}

echo "done\n";

?>
--EXPECT--
Write data from the file:
int(9000)

Read lines from the client

Close the server side socket and read the remaining data from the client
done
