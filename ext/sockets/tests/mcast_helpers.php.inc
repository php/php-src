<?php
function checktimeout($sock, $limit) {
	$readfs = array($sock);
	$writefs = $exceptfs = array();
	if (socket_select($readfs, $writefs, $exceptfs, 0, $limit*1000) != 1) {
		die("Socket read timeout hit. Can be a bug, a test bug, or a firewall issue.");
	}
}
