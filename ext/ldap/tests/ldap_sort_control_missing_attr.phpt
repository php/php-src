--TEST--
ldap_search(): malformed sort control (sort key missing "attr") must not free uninitialized memory
--EXTENSIONS--
ldap
--FILE--
<?php
// No server needed: the control array is validated before the search is sent.
// A sort key missing "attr" makes php_ldap_control_from_array() bail mid-loop;
// the failure cleanup must not walk/free the partially built sort_keys array.
$ld = ldap_connect("ldap://127.0.0.1:389");

try {
    ldap_search($ld, "dc=example,dc=com", "(objectClass=*)", [], 0, -1, -1, LDAP_DEREF_NEVER, [
        [
            'oid' => LDAP_CONTROL_SORTREQUEST,
            'value' => [
                ['attr' => 'cn'],
                ['reverse' => true],
            ],
        ],
    ]);
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "ok\n";
?>
--EXPECT--
ldap_search(): Sort key list must have an "attr" key
ok
