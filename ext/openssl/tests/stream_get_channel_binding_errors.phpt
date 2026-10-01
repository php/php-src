--TEST--
stream_get_channel_binding(): argument and type error handling
--EXTENSIONS--
openssl
--FILE--
<?php
/* Unknown channel binding type -> ValueError (checked before the stream). */
try {
    stream_get_channel_binding(fopen("php://memory", "r"), "not-a-type");
    echo "no error\n";
} catch (ValueError $e) {
    echo "ValueError\n";
}

/* Case-sensitivity: a mismatched case is also unknown. */
try {
    stream_get_channel_binding(fopen("php://memory", "r"), "TLS-UNIQUE");
    echo "no error\n";
} catch (ValueError $e) {
    echo "ValueError (case sensitive)\n";
}

/* Non-stream argument -> TypeError. */
try {
    stream_get_channel_binding(123, "tls-unique");
    echo "no error\n";
} catch (TypeError $e) {
    echo "TypeError\n";
}

/* A stream without transport encryption -> RuntimeException. */
$plain = fopen("php://memory", "r");
foreach (["tls-unique", "tls-server-end-point", "tls-exporter"] as $t) {
    try {
        stream_get_channel_binding($plain, $t);
        echo "$t: no error\n";
    } catch (RuntimeException $e) {
        echo "$t: RuntimeException\n";
    }
}
fclose($plain);
?>
--EXPECT--
ValueError
ValueError (case sensitive)
TypeError
tls-unique: RuntimeException
tls-server-end-point: RuntimeException
tls-exporter: RuntimeException
