--TEST--
Empty extension loading should not generate warnings
--FILE--
<?php

function testEmptyExtension($type, $value) {
    $cmd = [
        PHP_BINARY, '-n',
        '-dextension_dir=' . ini_get('extension_dir'),
        '-d' . $type . '=' . $value,
        '-r', 'echo "OK\n";'
    ];
    $proc = proc_open($cmd, [['null'], ['pipe', 'w'], ['pipe', 'w']], $pipes);
    $stdout = stream_get_contents($pipes[1]);
    $stderr = stream_get_contents($pipes[2]);
    proc_close($proc);
    
    if ($stderr) {
        echo "STDERR: " . trim($stderr) . "\n";
    }
    echo trim($stdout) . "\n";
}

echo "Testing empty extension directive:\n";
testEmptyExtension('extension', '');

echo "Testing empty zend_extension directive:\n";
testEmptyExtension('zend_extension', '');

?>
--EXPECT--
Testing empty extension directive:
OK
Testing empty zend_extension directive:
OK
