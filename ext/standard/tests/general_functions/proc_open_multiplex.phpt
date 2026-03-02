--TEST--
Multiplexing of child output
--SKIPIF--
<?php
if (getenv('GITHUB_ACTIONS') && str_starts_with(php_uname('m'), 'ppc')) {
    die('skip Flaky on GitHub Actions PPC runner');
}
?>
--FILE--
<?php
$php = getenv("TEST_PHP_EXECUTABLE");
$desc = [
    ["null"],
    ["pipe", "w"],
    ["null"]
];
$read_pipes = [];
for ($i = 0; $i < 10; $i++) {
    $procs[] = proc_open([$php, "-r", "usleep(10000 * (10 - $i)); echo 'hello$i';"], $desc, $pipes);
    $read_pipes[] = $pipes[1];
}
$out = [];
$rset = $read_pipes;
$wset = null;
$eset = null;
while (!empty($read_pipes) && ($selected = stream_select($rset, $wset, $eset, 1)) > 0) {
    foreach ($rset as $pipe) {
        if ($selected === 10) {
            echo "stream_select() reported all pipes as ready\n";
            echo "Read:\n", implode("\n", $out);
            exit;
        }
        $out[] = fread($pipe, 6);
        unset($read_pipes[array_search($pipe, $read_pipes)]);
    }
    $rset = $read_pipes;
}
if ($selected === false) {
    echo "stream_select() failed\n";
    echo "Read:\n", implode("\n", $out);
    exit;
}
sort($out);
echo "Read:\n", implode("\n", $out);
?>
--EXPECT--
Read:
hello0
hello1
hello2
hello3
hello4
hello5
hello6
hello7
hello8
hello9
