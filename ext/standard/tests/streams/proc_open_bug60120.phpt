--TEST--
Bug #60120 proc_open hangs with stdin/out with >2048 bytes
--FILE--
<?php
error_reporting(E_ALL);

$file = preg_replace(
    "~\.phpt?$~", ".io.php", __FILE__);

file_put_contents($file, <<<TMPFILE
<?php
\$input = stream_get_contents(STDIN);

if (\$input) {
    fwrite(STDOUT, \$input);
    fwrite(STDERR, \$input);
}
?>
TMPFILE
);

$command = sprintf("%s -n %s", PHP_BINARY, $file);

$process = proc_open(
    $command,
    [
        ['pipe', 'r'],
        ['pipe', 'w'],
        ['pipe', 'w']
    ],
    $pipes,
    getcwd(),
    [],
    [
        'suppress_errors' => true, 
        'bypass_shell' => false
    ]
);

if (!is_resource($process)) {
    die(sprintf(
        "could not open process \"%s\"", 
        $command));
}

fwrite($pipes[0], str_repeat('*', 10000));
fclose($pipes[0]);

stream_set_blocking($pipes[1], false);
stream_set_blocking($pipes[2], false);

$buffers = [
    1 => "",
    2 => ""
];

do {
    $r = [$pipes[1], $pipes[2]];
    $w = [];
    $e = [];
    $s = stream_select($r, $w, $e, 60);

    if (!$s) {
        if ($s === false) {
            proc_terminate($process);
        }
        break;
    }

    foreach ($r as $ready) {
        $buffers[
            array_search($ready, $pipes)
        ] .= fread($ready, 8192);
    }

    if (strlen($buffers[1]) === 10000 &&
        strlen($buffers[2]) === 10000) {
        break;
    }
} while (1);

var_dump(
    $buffers[1],
    $buffers[2],
    fread($pipes[1], 1),
    fread($pipes[2], 1));

fclose($pipes[1]);
fclose($pipes[2]);
?>
--EXPECTF--
string(10000) "%s"
string(10000) "%s"
string(0) ""
string(0) ""
--CLEAN--
unlink($file);
