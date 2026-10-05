--TEST--
openssl_get_channel_binding(): argument and type error handling
--EXTENSIONS--
openssl
--FILE--
<?php
/* Unknown channel binding type -> ValueError (checked before the stream). */
try {
    openssl_get_channel_binding(fopen("php://memory", "r"), "not-a-type");
    echo "no error\n";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* Case-sensitivity: a mismatched case is also unknown. */
try {
    openssl_get_channel_binding(fopen("php://memory", "r"), "TLS-UNIQUE");
    echo "no error\n";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* Non-stream argument -> TypeError. */
try {
    openssl_get_channel_binding(123, "tls-unique");
    echo "no error\n";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* A stream without transport encryption -> RuntimeException. */
$plain = fopen("php://memory", "r");
foreach (["tls-unique", "tls-server-end-point", "tls-exporter"] as $t) {
    try {
        openssl_get_channel_binding($plain, $t);
        echo "$t: no error\n";
    } catch (Throwable $e) {
        echo $t, ': ', $e::class, ': ', $e->getMessage(), "\n";
    }
}
fclose($plain);
?>
--EXPECT--
ValueError: openssl_get_channel_binding(): argument #2 ($channel_binding_type) "not-a-type" is not a known channel binding type, expected "tls-unique", "tls-server-end-point" or "tls-exporter"
ValueError: openssl_get_channel_binding(): argument #2 ($channel_binding_type) "TLS-UNIQUE" is not a known channel binding type, expected "tls-unique", "tls-server-end-point" or "tls-exporter"
TypeError: openssl_get_channel_binding(): Argument #1 ($stream) must be of type resource, int given
tls-unique: Openssl\OpensslException: Stream does not have transport encryption enabled
tls-server-end-point: Openssl\OpensslException: Stream does not have transport encryption enabled
tls-exporter: Openssl\OpensslException: Stream does not have transport encryption enabled
