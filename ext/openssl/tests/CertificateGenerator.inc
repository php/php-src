<?php

class CertificateGenerator
{
    const CONFIG = __DIR__. DIRECTORY_SEPARATOR . 'openssl.cnf';

    /** @var resource */
    private $ca;

    /** @var resource */
    private $caKey;

    /** @var resource|null */
    private $lastCert;

    /** @var resource|null */
    private $lastKey;

    public function __construct()
    {
        if (!extension_loaded('openssl')) {
            throw new RuntimeException(
                'openssl extension must be loaded to generate certificates'
            );
        }
        $this->generateCa();
    }

    /**
     * @param int|null $keyLength
     * @return resource
     */
    private static function generateKey($keyLength = null)
    {
        if (null === $keyLength) {
            $keyLength = 2048;
        }

        return openssl_pkey_new([
            'private_key_bits' => $keyLength,
            'private_key_type' => OPENSSL_KEYTYPE_RSA,
            'encrypt_key' => false,
        ]);
    }

    private function generateCa()
    {
        $this->caKey = self::generateKey();
        $dn = [
            'countryName' => 'GB',
            'stateOrProvinceName' => 'Berkshire',
            'localityName' => 'Newbury',
            'organizationName' => 'Example Certificate Authority',
            'commonName' => 'CA for PHP Tests'
        ];

        $this->ca = openssl_csr_sign(
            openssl_csr_new(
                $dn,
                $this->caKey,
                [
                    'x509_extensions' => 'v3_ca',
                    'config' => self::CONFIG,
                ]
            ),
            null,
            $this->caKey,
            2
        );
    }

    public function getCaCert()
    {
        $output = '';
        openssl_x509_export($this->ca, $output);

        return $output;
    }

    public function saveCaCert($file)
    {
        openssl_x509_export_to_file($this->ca, $file);
    }

    public function saveNewCertAsFileWithKey($commonNameForCert, $file, $keyLength = null)
    {
        $dn = [
            'countryName' => 'BY',
            'stateOrProvinceName' => 'Minsk',
            'localityName' => 'Minsk',
            'organizationName' => 'Example Org',
            'commonName' => $commonNameForCert,
        ];

        $this->lastKey = self::generateKey($keyLength);
        $this->lastCert = openssl_csr_sign(
            openssl_csr_new($dn, $this->lastKey, ['req_extensions' => 'v3_req']),
            $this->ca,
            $this->caKey,
            2
        );

        $certText = '';
        openssl_x509_export($this->lastCert, $certText);

        $keyText = '';
        openssl_pkey_export($this->lastKey, $keyText);

        file_put_contents($file, $certText . PHP_EOL . $keyText);
    }

    public function getCertDigest($algo)
    {
        return openssl_x509_fingerprint($this->lastCert, $algo);
    }
}
