<?php
class LocalSoapClient extends SoapClient {
	public function __doRequest(string $request, string $location, string $action, int $version, bool $oneWay = false): ?string {
		$doc = new DOMDocument();
		$doc->preserveWhiteSpace = false;
		$doc->formatOutput = true;
		$doc->loadXML($request);
		die($doc->saveXML());
	}
}
