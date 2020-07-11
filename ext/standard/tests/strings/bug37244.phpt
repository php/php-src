--TEST--
Bug #37244 (base64_decode violates RFC 3548)
--FILE--
<?php
$strings = array(
    'SW1wbGVtZW50YXRpb25zIE1VU1QgcmVqZWN0IHRoZSBlbmNvZGluZyBpZiBpdCBjb250YWlucyBjaGFyYWN0ZXJzIG91dHNpZGUgdGhlIGJhc2UgYWxwaGFiZXQu',
    'SW1wbGVtZW$0YXRpb25zIE1VU1QgcmVqZWN0IHRoZSBlbmNvZGluZyBpZiBpdCBjb250YWlucyBjaGFyYWN0ZXJzIG91dHNpZGUgdGhlIGJhc2UgYWxwaGFiZXQu',
    'SW1wbGVtZW0YXRpb25zIE1VU1QgcmVqZWN0IHRoZSBlbmNvZGluZyBpZiBpdCBjb250YWlucyBjaGFyYWN0ZXJzIG91dHNpZGUgdGhlIGJhc2UgYWxwaGFiZXQu'
);
foreach($strings as $string) {
    var_dump(base64_decode($string, true));
}
?>
--EXPECT--
string(93) "Implementations MUST reject the encoding if it contains characters outside the base alphabet."
bool(false)
string(92) "Implemem][ۜ�UT��Z�X�H[���[��Y�]�۝Z[���\�X�\���]�YHH�\�H[X�]"
