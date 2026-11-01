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
	'curl_setopt(): The CURLOPT_HTTP200ALIASES option must have an array value',
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
	    '/curl_setopt\(\): The CURLOPT_([A-Z_])+ option must have an array value/',
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
Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_AUTOREFERER succeeded
Option CURLOPT_BINARYTRANSFER succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_BUFFERSIZE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_CONNECTTIMEOUT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_COOKIESESSION succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_CRLF succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_DNS_CACHE_TIMEOUT succeeded
Option CURLOPT_DNS_USE_GLOBAL_CACHE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FAILONERROR succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FILETIME succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FOLLOWLOCATION succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FORBID_REUSE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FRESH_CONNECT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTPAPPEND succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTPLISTONLY succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTP_USE_EPRT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTP_USE_EPSV succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HEADER succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HTTPGET succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HTTPPROXYTUNNEL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HTTP_VERSION succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_INFILESIZE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_INFILESIZE_LARGE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_LOW_SPEED_LIMIT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_LOW_SPEED_TIME succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MAXCONNECTS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MAXREDIRS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_NETRC succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_NOBODY succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_NOPROGRESS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_NOSIGNAL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PORT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_POST succeeded
Option CURLOPT_PRIVATE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PROXYPORT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PROXYTYPE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PUT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_RESUME_FROM succeeded
Option CURLOPT_RETURNTRANSFER succeeded
Option CURLOPT_SHARE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSLVERSION succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d

Notice: curl_setopt(): CURLOPT_SSL_VERIFYHOST no longer accepts the value 1, value 2 will be used instead in %s on line %d
Option CURLOPT_SSL_VERIFYHOST succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSL_VERIFYPEER succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TCP_KEEPCNT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TIMECONDITION succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TIMEOUT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TIMEVALUE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TRANSFERTEXT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_UNRESTRICTED_AUTH succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_UPLOAD succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_VERBOSE succeeded
Option CURLFOLLOW_OBEYCODE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLFOLLOW_FIRSTONLY succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLINFO_DATA_IN succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_ABORTED_BY_CALLBACK succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_BAD_CALLING_ORDER succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_BAD_CONTENT_ENCODING succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_BAD_FUNCTION_ARGUMENT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_BAD_PASSWORD_ENTERED succeeded
Option CURLE_FAILED_INIT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_FTP_COULDNT_GET_SIZE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_FTP_COULDNT_RETR_FILE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_FTP_QUOTE_ERROR succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_FTP_WEIRD_227_FORMAT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_FTP_WEIRD_PASV_REPLY succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_FTP_WRITE_ERROR succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_FUNCTION_NOT_FOUND succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_GOT_NOTHING succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_HTTP_PORT_FAILED succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_HTTP_POST_ERROR succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_HTTP_RANGE_ERROR succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_OBSOLETE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_OUT_OF_MEMORY succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_SSL_CERTPROBLEM succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_SSL_CIPHER succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_SSL_ENGINE_NOTFOUND succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_SSL_ENGINE_SETFAILED succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_TOO_MANY_REDIRECTS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_UNKNOWN_TELNET_OPTION succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_URL_MALFORMAT succeeded
Option CURLINFO_HEADER_OUT succeeded
Option CURLM_BAD_EASY_HANDLE succeeded
Option CURLM_CALL_MULTI_PERFORM succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLM_OUT_OF_MEMORY succeeded
Option CURLSHOPT_UNSHARE succeeded
Option CURL_HTTP_VERSION_1_1 succeeded
Option CURL_LOCK_DATA_COOKIE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURL_LOCK_DATA_DNS succeeded
Option CURL_NETRC_REQUIRED succeeded
Option CURL_SSLVERSION_SSLv2 succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURL_SSLVERSION_SSLv3 succeeded
Option CURL_TIMECOND_IFUNMODSINCE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURL_TIMECOND_LASTMOD succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURL_VERSION_DEBUG succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURL_VERSION_GSSNEGOTIATE succeeded
Option CURL_VERSION_KERBEROS4 succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HTTPAUTH succeeded
Option CURLAUTH_DIGEST succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTP_CREATE_MISSING_DIRS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PROXYAUTH succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTP_RESPONSE_TIMEOUT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SERVER_RESPONSE_TIMEOUT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_IPRESOLVE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MAXFILESIZE succeeded
Option CURL_IPRESOLVE_V6 succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLE_FTP_SSL_FAILED succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLFTPSSL_ALL succeeded
Option CURLFTPSSL_CONTROL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTP_SSL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MAXFILESIZE_LARGE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TCP_NODELAY succeeded
Option CURLFTPAUTH_TLS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTPSSLAUTH succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_IGNORE_CONTENT_LENGTH succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTP_SKIP_PASV_IP succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTP_FILEMETHOD succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_CONNECT_ONLY succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_LOCALPORT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_LOCALPORTRANGE succeeded
Option CURLFTPMETHOD_NOCWD succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLFTPMETHOD_SINGLECWD succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MAX_RECV_SPEED_LARGE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MAX_SEND_SPEED_LARGE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSL_SESSIONID_CACHE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLMOPT_PIPELINING succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTP_SSL_CCC succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSH_AUTH_TYPES succeeded
Option CURLFTPSSL_CCC_ACTIVE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_CONNECTTIMEOUT_MS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HTTP_CONTENT_DECODING succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HTTP_TRANSFER_DECODING succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TIMEOUT_MS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_NEW_DIRECTORY_PERMS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_NEW_FILE_PERMS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_APPEND succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_DIRLISTONLY succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_USE_SSL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLUSESSL_ALL succeeded
Option CURLUSESSL_CONTROL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PROXY_TRANSFER_MODE succeeded
Option CURL_SEEKFUNC_CANTSEEK succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_ADDRESS_SCOPE succeeded
Option CURLSSH_AUTH_ANY succeeded
Option CURLSSH_AUTH_DEFAULT succeeded
Option CURLSSH_AUTH_PASSWORD succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_CERTINFO succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_POSTREDIR succeeded
Option CURL_REDIR_POST_302 succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PROTOCOLS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_REDIR_PROTOCOLS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SOCKS5_GSSAPI_NEC succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TFTP_BLKSIZE succeeded
Option CURLPROTO_ALL succeeded
Option CURLPROTO_HTTPS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPROTO_SFTP succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPROTO_TELNET succeeded
Option CURLFTP_CREATE_DIR_RETRY succeeded
Option CURLKHMATCH_MISSING succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLKHMATCH_LAST succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_FTP_USE_PRET succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_RTSP_CLIENT_CSEQ succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_RTSP_REQUEST succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_RTSP_SERVER_CSEQ succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURL_RTSPREQ_ANNOUNCE succeeded
Option CURL_RTSPREQ_DESCRIBE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_WILDCARDMATCH succeeded
Option CURL_FNMATCHFUNC_FAIL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLAUTH_NTLM_WB succeeded
Option CURLGSSAPI_DELEGATION_FLAG succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_GSSAPI_DELEGATION succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_ACCEPTTIMEOUT_MS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSL_OPTIONS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TCP_KEEPALIVE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TCP_KEEPIDLE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TCP_KEEPINTVL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLMOPT_MAX_TOTAL_CONNECTIONS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SASL_IR succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURL_HTTP_VERSION_2_0 succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_EXPECT_100_TIMEOUT_MS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSL_ENABLE_ALPN succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSL_ENABLE_NPN succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HEADEROPT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSL_VERIFYSTATUS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PATH_AS_IS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSL_FALSESTART failed

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURL_HTTP_VERSION_2 succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PIPEWAIT succeeded
Option CURLPIPE_MULTIPLEX succeeded
Option CURLSSLOPT_NO_REVOKE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_STREAM_WEIGHT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TFTP_NO_OPTIONS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TCP_FASTOPEN succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_KEEP_SENDING_ON_ERROR succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PROXY_SSL_OPTIONS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PROXY_SSL_VERIFYHOST succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PROXY_SSL_VERIFYPEER succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_PROXY_SSLVERSION succeeded
Option CURLPROXY_HTTPS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SUPPRESS_CONNECT_HEADERS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SOCKS5_AUTH succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_SSH_COMPRESSION succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLSSH_AUTH_GSSAPI succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_TIMEVALUE_LARGE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_DNS_SHUFFLE_ADDRESSES succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HAPROXYPROTOCOL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLAUTH_BEARER succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_DISALLOW_USERNAME_IN_URL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_UPKEEP_INTERVAL_MS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_UPLOAD_BUFFERSIZE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HTTP09_ALLOWED succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLALTSVC_H3 succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_ALTSVC_CTRL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MAXAGE_CONN succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MAIL_RCPT_ALLLOWFAILS succeeded
Option CURLPX_BAD_VERSION succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPX_CLOSED succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPX_RECV_ADDRESS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPX_RECV_AUTH succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPX_REPLY_CONNECTION_REFUSED succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPX_REPLY_GENERAL_SERVER_FAILURE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPX_REPLY_HOST_UNREACHABLE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPX_RESOLVE_HOST succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPX_UNKNOWN_MODE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLPX_USER_REJECTED succeeded
Option CURLHSTS_READONLYFILE succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_HSTS_CTRL succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_DOH_SSL_VERIFYHOST succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_DOH_SSL_VERIFYPEER succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_DOH_SSL_VERIFYSTATUS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLSSLOPT_AUTO_CLIENT_CERT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MAXLIFETIME_CONN succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_MIME_OPTIONS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_WS_OPTIONS succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_CA_CACHE_TIMEOUT succeeded

Warning: Object of class stdClass could not be converted to int in %s on line %d
Option CURLOPT_QUICK_EXIT succeeded
Option CURLOPT_SAFE_UPLOAD succeeded
