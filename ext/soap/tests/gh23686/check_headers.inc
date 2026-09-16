<?php

include __DIR__ . "/../../../../sapi/cli/tests/php_cli_server.inc";

function start_server(string $code, ?string $entry_point) {
	$args = [
		"-d",
		"extension_dir=" . ini_get("extension_dir"),
		"-d",
		"extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "soap." . PHP_SHLIB_SUFFIX,
	];
	if (php_ini_loaded_file()) {
		$args[] = "-c";
		$args[] = php_ini_loaded_file();
	}
	return @php_cli_server_start($code, $entry_point, $args);
}

/**
 * When an <import> switches to a different server, check the headers that the
 * second server recieves
 */
function check_headers_for_import(string $logs_path, $context) {
	$code = file_get_contents(__DIR__ . "/soap-respond-with-headers.inc");
	$code = substr($code, strpos($code, "<?php") + strlen("<?php"));
	$code = str_replace('"{LOG_FILE}"', var_export($logs_path, true), $code);

	$logging_server = start_server($code, null);

	$wsdl = file_get_contents(__DIR__ . "/wsdl-with-import.wsdl");
	$wsdl = str_replace("{TARGET_PORT}", $logging_server->port, $wsdl);

	// We cannot just load the SoapClient directly from the WSDL because
	// otherwise the credentials are not stripped because we are not switching
	// to another host in the <import>.
	$server_code = "echo <<<END\n$wsdl\nEND;";
	$wsdl_server = start_server($server_code, "router.php");
	$wsdl_server_port = $wsdl_server->port;

	foreach ([SOAP_1_1, SOAP_1_2] as $version) {
		try {
			$client = new SoapClient("http://localhost:$wsdl_server_port", [
				'soap_version' => $version,
				'stream_context' => $context,
			]);
		} catch (SoapFault $e) {
			var_dump($e);
			echo "--- Raw Server Response Body ---\n";
			var_dump(file_get_contents("http://localhost:$wsdl_server_port", false, $context));
		}

		var_dump(file_get_contents($logs_path));
	}
}

/**
 * When an <import> leads to a <schema> on a different server, check the
 * headers that the *third* server recieves
 */
function check_headers_for_schema(string $logs_path, $context) {
	$code = file_get_contents(__DIR__ . "/schema-respond-with-headers.inc");
	$code = substr($code, strpos($code, "<?php") + strlen("<?php"));
	$code = str_replace('"{LOG_FILE}"', var_export($logs_path, true), $code);

	$logging_server = start_server($code, null);

	$schema_wsdl = file_get_contents(__DIR__ . "/wsdl-with-schema.wsdl");
	$schema_wsdl = str_replace("{TARGET_PORT}", $logging_server->port, $schema_wsdl);
	$schema_server_code = "echo <<<END\n$schema_wsdl\nEND;";
	$schema_wsdl_server = start_server($schema_server_code, "schema.php");
	$schema_wsdl_server_port = $schema_wsdl_server->port;

	$import_wsdl = file_get_contents(__DIR__ . "/wsdl-with-import.wsdl");
	$import_wsdl = str_replace("{TARGET_PORT}", $schema_wsdl_server_port, $import_wsdl);
	$import_server_code = "echo <<<END\n$import_wsdl\nEND;";
	$import_wsdl_server = start_server($import_server_code, "router.php");
	$import_wsdl_server_port = $import_wsdl_server->port;

	foreach ([SOAP_1_1, SOAP_1_2] as $version) {
		try {
			$client = new SoapClient("http://localhost:$import_wsdl_server_port", [
				'soap_version' => $version,
				'stream_context' => $context,
			]);
		} catch (SoapFault $e) {
			var_dump($e);
			echo "--- Raw Server Response Body ---\n";
			var_dump(file_get_contents("http://localhost:$import_wsdl_server_port", false, $context));
		}

		var_dump(file_get_contents($logs_path));
	}
}
