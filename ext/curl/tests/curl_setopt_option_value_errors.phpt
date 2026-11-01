--TEST--
Check that curl_setopt() values are of the correct type for the given option
--EXTENSIONS--
curl
--FILE--
<?php

const KNOWNS_ERRORS = [
	'curl_setopt(): Argument #2 ($option) is not a valid cURL option',
	'curl_setopt(): supplied argument is not a valid File-Handle resource',
	'Object of class stdClass could not be converted to string',
	'curl_setopt(): Argument #3 ($value) must be of type CurlSharePersistentHandle|CurlShareHandle for option CURLOPT_SHARE, stdClass given',
];

function isKnownError($error): bool {
	//return in_array($error, KNOWNS_ERRORS);
	if (in_array($error, KNOWNS_ERRORS)) {
		return true;
	}
	if (preg_match(
	    '/curl_setopt\(\): Argument #3 \(\$value\) must be a valid callback for option CURLOPT_([A-Z_])+, no array or string given/',
	    $error,
	)) {
	    return true;
	}
	if (preg_match(
	    '/curl_setopt\(\): Argument #3 \(\$value\) must be of type array for option CURLOPT_([A-Z0-9_])+, stdClass given/',
	    $error,
	)) {
	    return true;
	}
	if (preg_match(
		'/curl_setopt\(\): Argument #3 \(\$value\) must be of type int for option CURLOPT_([A-Z0-9_])+, stdClass given/',
		$error,
	)) {
		return true;
	}
	return false;
}

$ch = curl_init();
$obj = new stdClass();

$re = new ReflectionExtension('curl');
$constants = $re->getConstants();

foreach ($constants as $name => $constant) {
	try {
	    $r = curl_setopt($ch, $constant, $obj);
	    if ($r === false) {
	        echo "Option $name failed\n";
	    } else {
	        echo "Option $name succeeded\n";
	    }
	} catch (Throwable $e) {
		if (!isKnownError($e->getMessage())) {
	        echo "Option $name failed with exception:\n";
	        echo $e::class, ': ', $e->getMessage(), "\n";
		}
	}
}

?>
--EXPECTF--
Option CURLOPT_BINARYTRANSFER succeeded
Option CURLOPT_DNS_USE_GLOBAL_CACHE succeeded
Option CURLOPT_PRIVATE succeeded
Option CURLOPT_RETURNTRANSFER succeeded
Option CURLFOLLOW_OBEYCODE succeeded
Option CURLE_FAILED_INIT succeeded
Option CURLINFO_HEADER_OUT succeeded
Option CURLM_BAD_EASY_HANDLE succeeded
Option CURLM_CALL_MULTI_PERFORM succeeded
Option CURLSHOPT_UNSHARE succeeded
Option CURL_HTTP_VERSION_1_1 succeeded
Option CURL_LOCK_DATA_COOKIE succeeded
Option CURL_NETRC_REQUIRED succeeded
Option CURL_SSLVERSION_SSLv2 succeeded
Option CURL_TIMECOND_IFUNMODSINCE succeeded
Option CURL_VERSION_KERBEROS4 succeeded
Option CURLAUTH_DIGEST succeeded
Option CURL_IPRESOLVE_V6 succeeded
Option CURLFTPSSL_CONTROL succeeded
Option CURLFTPAUTH_TLS succeeded
Option CURLFTPMETHOD_NOCWD succeeded
Option CURLFTPSSL_CCC_ACTIVE succeeded
Option CURLUSESSL_CONTROL succeeded
Option CURL_SEEKFUNC_CANTSEEK succeeded
Option CURLSSH_AUTH_ANY succeeded
Option CURLSSH_AUTH_DEFAULT succeeded
Option CURLSSH_AUTH_PASSWORD succeeded
Option CURL_REDIR_POST_302 succeeded
Option CURLPROTO_ALL succeeded
Option CURLPROTO_HTTPS succeeded
Option CURLFTP_CREATE_DIR_RETRY succeeded
Option CURLKHMATCH_MISSING succeeded
Option CURL_RTSPREQ_DESCRIBE succeeded
Option CURL_FNMATCHFUNC_FAIL succeeded
Option CURLGSSAPI_DELEGATION_FLAG succeeded
Option CURLPIPE_MULTIPLEX succeeded
Option CURLSSLOPT_NO_REVOKE succeeded
Option CURLPROXY_HTTPS succeeded
Option CURLPX_BAD_VERSION succeeded
Option CURLHSTS_READONLYFILE succeeded
Option CURLOPT_SAFE_UPLOAD succeeded
