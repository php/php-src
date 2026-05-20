--TEST--
Hash: CRC performance with hardware acceleration
--SKIPIF--
<?php
if (!extension_loaded('hash')) die('skip hash extension not available');

// Skip if crc-fast support is not compiled in
// We check this by looking for "crc-fast support => enabled" in phpinfo
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
if (!preg_match('/crc-fast support\s*=>\s*enabled/', $info)) {
    die('skip crc-fast support not enabled');
}

// Skip if not on supported hardware platforms (x86, x86_64, aarch64)
$arch = strtolower(php_uname('m'));

// Normalize architecture names (following PHP's run-extra-tests.php pattern)
$is_x86_64 = in_array($arch, ['x86_64', 'amd64']);
$is_aarch64 = in_array($arch, ['aarch64', 'arm64']);

// technically, i686 w/SSE2 should also work, but it's difficult to detect and rare
if (!($is_x86_64 || $is_aarch64)) {
    die('skip crc-fast hardware acceleration not supported on ' . $arch);
}
?>
--FILE--
<?php
// Test that CRC algorithms achieve >10 GiB/s throughput with hardware acceleration (software is currently at ~0.5GiB/s)
$algorithms = [
    'crc32-iso-hdlc',
    'crc32-iscsi',
    'crc32',  // Standard PHP crc32
    'crc32-php',
    'crc64-nvme',
    'crc64-ecma-182'
];

$data_size = 1048576; // 1 MiB
$test_data = (new \Random\Randomizer())->getBytes($data_size); 
$min_throughput_gibps = 10; // Minimum 10 GiB/s

echo "Testing CRC performance with hardware acceleration\n";
echo "Minimum required throughput: $min_throughput_gibps GiB/s\n\n";

$all_passed = true;

foreach ($algorithms as $algo) {
    $start = microtime(true);
    
    $hash = hash($algo, $test_data);
    
    $elapsed = microtime(true) - $start;
    
    $throughput_mbps = ($data_size / $elapsed) / 1048576;
    $throughput_gibps = $throughput_mbps / 1024;
    
    $status = $throughput_gibps >= $min_throughput_gibps ? 'PASS' : 'FAIL';
    
    printf("%-20s: %.2f GiB/s - %s\n", $algo, $throughput_gibps, $status);
    
    if ($status === 'FAIL') {
        $all_passed = false;
    }
}

echo "\n";
if ($all_passed) {
    echo "All CRC algorithms meet performance requirements\n";
} else {
    echo "Some CRC algorithms failed to meet performance requirements\n";
}

// Also verify correctness with known test vectors
echo "\nVerifying correctness with test vector '123456789':\n";
$test_vector = "123456789";
$expected = [
    'crc32-iso-hdlc' => 'cbf43926',
    'crc32-iscsi' => 'e3069283',
    'crc32' => '181989fc',
    'crc32-php' => '181989fc',
    'crc64-nvme' => 'ae8b14860a799888',
    'crc64-ecma-182' => '6c40df5f0b497347'
];

$correct = true;
foreach ($expected as $algo => $expected_hash) {
    $actual = hash($algo, $test_vector);
    if ($actual === $expected_hash) {
        echo "$algo: OK\n";
    } else {
        echo "$algo: FAILED (expected: $expected_hash, got: $actual)\n";
        $correct = false;
    }
}

if ($correct) {
    echo "\nAll checksums correct\n";
} else {
    echo "\nChecksum verification failed\n";
}
?>
--EXPECTF--
Testing CRC performance with hardware acceleration
Minimum required throughput: 10 GiB/s

crc32-iso-hdlc      : %f GiB/s - PASS
crc32-iscsi         : %f GiB/s - PASS
crc32               : %f GiB/s - PASS
crc32-php           : %f GiB/s - PASS
crc64-nvme          : %f GiB/s - PASS
crc64-ecma-182      : %f GiB/s - PASS

All CRC algorithms meet performance requirements

Verifying correctness with test vector '123456789':
crc32-iso-hdlc: OK
crc32-iscsi: OK
crc32: OK
crc32-php: OK
crc64-nvme: OK
crc64-ecma-182: OK

All checksums correct
