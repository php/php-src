--TEST--
Hash: serialization errors
--FILE--
<?php

// cannot unserialize onto an already-initialized object
$ctx = hash_init("sha256");
try {
    $ctx->__unserialize($ctx->__serialize());
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// bad formats
foreach ([
    "TzoxMToiSGFzaENvbnRleHQiOjA6e30=", // no contents
    "TzoxMToiSGFzaENvbnRleHQiOjU6e2k6MDtpOjE7aToxO2k6MDtpOjI7YTo4OntpOjA7aToxNzMyNTg0MTkzO2k6MTtpOi0yNzE3MzM4Nzk7aToyO2k6LTE3MzI1ODQxOTQ7aTozO2k6MjcxNzMzODc4O2k6NDtpOi0xMDA5NTg5Nzc2O2k6NTtpOjIwMDtpOjY7aTowO2k6NztzOjY0OiJJIGNhbid0IHJlbWVtYmVyIGFueXRoaW5nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIjt9aTozO2k6MjtpOjQ7YTowOnt9fQ==", // algorithm is int
    "TzoxMToiSGFzaENvbnRleHQiOjU6e2k6MDtzOjQ6InNoYTEiO2k6MTtzOjA6IiI7aToyO2E6ODp7aTowO2k6MTczMjU4NDE5MztpOjE7aTotMjcxNzMzODc5O2k6MjtpOi0xNzMyNTg0MTk0O2k6MztpOjI3MTczMzg3ODtpOjQ7aTotMTAwOTU4OTc3NjtpOjU7aToyMDA7aTo2O2k6MDtpOjc7czo2NDoiSSBjYW4ndCByZW1lbWJlciBhbnl0aGluZwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACI7fWk6MztpOjI7aTo0O2E6MDp7fX0=", // flags are string
    "TzoxMToiSGFzaENvbnRleHQiOjU6e2k6MDtzOjQ6InNoYTEiO2k6MTtpOjE7aToyO2E6ODp7aTowO2k6MTczMjU4NDE5MztpOjE7aTotMjcxNzMzODc5O2k6MjtpOi0xNzMyNTg0MTk0O2k6MztpOjI3MTczMzg3ODtpOjQ7aTotMTAwOTU4OTc3NjtpOjU7aToyMDA7aTo2O2k6MDtpOjc7czo2NDoiSSBjYW4ndCByZW1lbWJlciBhbnl0aGluZwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACI7fWk6MztpOjI7aTo0O2E6MDp7fX0=", // flags indicate HASH_HMAC
    "TzoxMToiSGFzaENvbnRleHQiOjU6e2k6MDtzOjQ6InNoYTEiO2k6MTtpOjI7aToyO3M6MTA6ImFiY2RlZmdoaWoiO2k6MztpOjI7aTo0O2E6MDp7fX0=", // serialization format wrong
    "TzoxMToiSGFzaENvbnRleHQiOjU6e2k6MDtzOjQ6InNoYTEiO2k6MTtpOjA7aToyO2E6ODp7aTowO2k6MTczMjU4NDE5MztpOjE7aTotMjcxNzMzODc5O2k6MjtpOi0xNzMyNTg0MTk0O2k6MztpOjI3MTczMzg3ODtpOjQ7aTotMTAwOTU4OTc3NjtpOjU7aToyMDA7aTo2O3M6MDoiIjtpOjc7czo2NDoiSSBjYW4ndCByZW1lbWJlciBhbnl0aGluZwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACI7fWk6MztpOjI7aTo0O2E6MDp7fX0=", // serialization internals wrong
    "TzoxMToiSGFzaENvbnRleHQiOjU6e2k6MDtzOjQ6InNoYTEiO2k6MTtpOjA7aToyO2E6ODp7aTowO2k6MTczMjU4NDE5MztpOjE7aTotMjcxNzMzODc5O2k6MjtpOi0xNzMyNTg0MTk0O2k6MztpOjI3MTczMzg3ODtpOjQ7aTotMTAwOTU4OTc3NjtpOjU7aToyMDA7aTo2O2k6MDtpOjc7czo2NDoiSSBjYW4ndCByZW1lbWJlciBhbnl0aGluZwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACI7fWk6MztpOjA7aTo0O2E6MDp7fX0=", // bad magic number
    "TzoxMToiSGFzaENvbnRleHQiOjU6e2k6MDtzOjQ6Inh4eDEiO2k6MTtpOjA7aToyO2E6ODp7aTowO2k6MTczMjU4NDE5MztpOjE7aTotMjcxNzMzODc5O2k6MjtpOi0xNzMyNTg0MTk0O2k6MztpOjI3MTczMzg3ODtpOjQ7aTotMTAwOTU4OTc3NjtpOjU7aToyMDA7aTo2O2k6MDtpOjc7czo2NDoiSSBjYW4ndCByZW1lbWJlciBhbnl0aGluZwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACI7fWk6MztpOjI7aTo0O2E6MDp7fX0=" // bad algorithm
] as $serial) {
    try {
        $ctx = unserialize(base64_decode($serial));
        echo "Unexpected success\n";
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "Done\n";
?>
--EXPECT--
HashContext::__unserialize called on initialized object
Incomplete or ill-formed serialization data
Incomplete or ill-formed serialization data
Incomplete or ill-formed serialization data
HashContext with HASH_HMAC option cannot be serialized
Incomplete or ill-formed serialization data ("sha1" code -1)
Incomplete or ill-formed serialization data ("sha1" code -1024)
Incomplete or ill-formed serialization data ("sha1" code -1)
Unknown hash algorithm
Done
