if(!empty($_REQUEST["redir"])) {
	echo "REDIRECTED\n";
	return;
}

if(!empty($_REQUEST["loc"])) {
	header("Location: index.php?redir=1");
}

if(!empty($_REQUEST["status"])) {
	http_response_code($_REQUEST["status"]);
}

echo "HELLO!\n";