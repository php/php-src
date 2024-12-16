--TEST--
Multiplexing of child output
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
    $procs[] = proc_open([$php, "-r", "usleep(100000 * (10 - $i)); echo 'hello$i';"], $desc, $pipes);
    $read_pipes[] = $pipes[1];
}
$rset = $read_pipes;
$wset = null;
$eset = null;
while (!empty($read_pipes) && stream_select($rset, $wset, $eset, 2) > 0) {
    foreach ($rset as $pipe) {
        echo fread($pipe, 6), "\n";
        unset($read_pipes[array_search($pipe, $read_pipes)]);
    }
    $rset = $read_pipes;
}
?>
--EXPECT--
hello9
hello8
hello7
hello6
hello5
hello4
hello3
hello2
hello1
hello0
