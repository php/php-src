<?php
$php_openssl_cipher_tests = array(
    'aes-256-ccm' => array(
        array(
            'key' => '1bde3251d41a8b5ea013c195ae128b21' .
                     '8b3e0306376357077ef1c1c78548b92e',
            'iv'  => '5b8e40746f6b98e00f1d13ff41',
            'aad' => 'c17a32514eb6103f3249e076d4c871dc' .
                     '97e04b286699e54491dc18f6d734d4c0',
            'tag' => '2024931d73bca480c24a24ece6b6c2bf',
            'pt'  => '53bd72a97089e312422bf72e242377b3' .
                     'c6ee3e2075389b999c4ef7f28bd2b80a',
            'ct'  => '9a5fcccdb4cf04e7293d2775cc76a488' .
                     'f042382d949b43b7d6bb2b9864786726',
        ),
    ),
    'aes-128-gcm' => array(
        array(
            'key' => '00000000000000000000000000000000',
            'iv'  => '000000000000000000000000',
            'tag' => '58e2fccefa7e3061367f1d57a4e7455a',
            'pt'  => '',
            'ct'  => '',
        ),
        array(
            'key' => '00000000000000000000000000000000',
            'iv'  => '000000000000000000000000',
            'tag' => 'ab6e47d42cec13bdf53a67b21257bddf',
            'pt'  => '00000000000000000000000000000000',
            'ct'  => '0388dace60b6a392f328c2b971b2fe78',
        ),
        array(
            'key' => 'feffe9928665731c6d6a8f9467308308',
            'iv'  => 'cafebabefacedbaddecaf888',
            'tag' => '4d5c2af327cd64a62cf35abd2ba6fab4',
            'pt'  => 'd9313225f88406e5a55909c5aff5269a' .
                     '86a7a9531534f7da2e4c303d8a318a72' .
                     '1c3c0c95956809532fcf0e2449a6b525' .
                     'b16aedf5aa0de657ba637b391aafd255',
            'ct'  => '42831ec2217774244b7221b784d0d49c' .
                     'e3aa212f2c02a4e035c17e2329aca12e' .
                     '21d514b25466931c7d8f6a5aac84aa05' .
                     '1ba30b396a0aac973d58e091473f5985',
        ),
        array(
            'key' => 'feffe9928665731c6d6a8f9467308308',
            'iv'  => 'cafebabefacedbaddecaf888',
            'aad' => 'feedfacedeadbeeffeedfacedeadbeefabaddad2',
            'tag' => '5bc94fbc3221a5db94fae95ae7121a47',
            'pt'  => 'd9313225f88406e5a55909c5aff5269a' .
                     '86a7a9531534f7da2e4c303d8a318a72' .
                     '1c3c0c95956809532fcf0e2449a6b525' .
                     'b16aedf5aa0de657ba637b39',
            'ct'  => '42831ec2217774244b7221b784d0d49c' .
                     'e3aa212f2c02a4e035c17e2329aca12e' .
                     '21d514b25466931c7d8f6a5aac84aa05' .
                     '1ba30b396a0aac973d58e091',
        ),
        array(
            'key' => 'feffe9928665731c6d6a8f9467308308',
            'iv'  => 'cafebabefacedbad',
            'aad' => 'feedfacedeadbeeffeedfacedeadbeefabaddad2',
            'tag' => '3612d2e79e3b0785561be14aaca2fccb',
            'pt'  => 'd9313225f88406e5a55909c5aff5269a' .
                     '86a7a9531534f7da2e4c303d8a318a72' .
                     '1c3c0c95956809532fcf0e2449a6b525' .
                     'b16aedf5aa0de657ba637b39',
            'ct'  => '61353b4c2806934a777ff51fa22a4755' .
                     '699b2a714fcdc6f83766e5f97b6c7423' .
                     '73806900e49f24b22b097544d4896b42' .
                     '4989b5e1ebac0f07c23f4598'
        ),
        array(
            'key' => 'feffe9928665731c6d6a8f9467308308',
            'iv'  => '9313225df88406e555909c5aff5269aa' .
                     '6a7a9538534f7da1e4c303d2a318a728' .
                     'c3c0c95156809539fcf0e2429a6b5254' .
                     '16aedbf5a0de6a57a637b39b',
            'aad' => 'feedfacedeadbeeffeedfacedeadbeefabaddad2',
            'tag' => '619cc5aefffe0bfa462af43c1699d050',
            'pt'  => 'd9313225f88406e5a55909c5aff5269a' .
                     '86a7a9531534f7da2e4c303d8a318a72' .
                     '1c3c0c95956809532fcf0e2449a6b525' .
                     'b16aedf5aa0de657ba637b39',
            'ct'  => '8ce24998625615b603a033aca13fb894' .
                     'be9112a5c3a211a8ba262a3cca7e2ca7' .
                     '01e4a9a4fba43c90ccdcb281d48c7c6f' .
                     'd62875d2aca417034c34aee5',
        ),
    )
);

function openssl_get_cipher_tests($method)
{
    global $php_openssl_cipher_tests;

    $tests = array();

    foreach ($php_openssl_cipher_tests[$method] as $instance) {
        $test = array();
        foreach ($instance as $field_name => $field_value) {
            $test[$field_name] = pack("H*", $field_value);
        }
        if (!isset($test['aad'])) {
            $test['aad'] = "";
        }
        $tests[] = $test;
    }

    return $tests;
}
