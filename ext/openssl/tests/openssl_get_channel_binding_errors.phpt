--TEST--
openssl_get_channel_binding(): argument and type error handling
--EXTENSIONS--
openssl
--FILE--
<?php
/* The enum exposes the three IANA names as backing values. */
foreach (Openssl\ChannelBindingType::cases() as $type) {
    echo $type->name, '=', $type->value, "\n";
}

/* A string is not accepted; the parameter requires the enum. */
try {
    openssl_get_channel_binding(fopen("php://memory", "r"), "tls-unique");
    echo "no error\n";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* An unknown string is not accepted either. */
try {
    openssl_get_channel_binding(fopen("php://memory", "r"), "not-a-type");
    echo "no error\n";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* Non-stream argument -> TypeError. */
try {
    openssl_get_channel_binding(123, Openssl\ChannelBindingType::TlsUnique);
    echo "no error\n";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* A stream without transport encryption -> Openssl\OpensslException. */
$plain = fopen("php://memory", "r");
foreach (Openssl\ChannelBindingType::cases() as $type) {
    try {
        openssl_get_channel_binding($plain, $type);
        echo $type->value, ": no error\n";
    } catch (Throwable $e) {
        echo $type->value, ': ', $e::class, ': ', $e->getMessage(), "\n";
    }
}
fclose($plain);
?>
--EXPECT--
TlsUnique=tls-unique
TlsServerEndPoint=tls-server-end-point
TlsExporter=tls-exporter
TypeError: openssl_get_channel_binding(): Argument #2 ($channel_binding_type) must be of type Openssl\ChannelBindingType, string given
TypeError: openssl_get_channel_binding(): Argument #2 ($channel_binding_type) must be of type Openssl\ChannelBindingType, string given
TypeError: openssl_get_channel_binding(): Argument #1 ($stream) must be of type resource, int given
tls-unique: Openssl\OpensslException: Stream does not have transport encryption enabled
tls-server-end-point: Openssl\OpensslException: Stream does not have transport encryption enabled
tls-exporter: Openssl\OpensslException: Stream does not have transport encryption enabled
