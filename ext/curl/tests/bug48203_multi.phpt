--TEST--
Variation of bug #48203 with curl_multi_exec (Crash when file pointers passed to curl are closed before calling curl_multi_exec)
--SKIPIF--
<?php
include 'skipif.inc';
?>
--FILE--
<?php
include 'server.inc';
function checkForClosedFilePointer($target_url, $curl_option, $description) {
    $fp = fopen(__DIR__ . '/bug48203.tmp', 'w');

    $ch1 = curl_init();
    $ch2 = curl_init();

    $options = array(
        CURLOPT_RETURNTRANSFER => 1,
        $curl_option => $fp,
        CURLOPT_URL => $target_url,
    );

    // we also need to set CURLOPT_VERBOSE to test CURLOPT_STDERR properly
    if (CURLOPT_STDERR == $curl_option) {
        $options[CURLOPT_VERBOSE] = 1;
    }

    if (CURLOPT_INFILE == $curl_option) {
        $options[CURLOPT_UPLOAD] = 1;
    }

    curl_setopt_array($ch1, $options);
    curl_setopt_array($ch2, $options);

    fclose($fp); // <-- premature close of $fp caused a crash!

    $mh = curl_multi_init();

    curl_multi_add_handle($mh, $ch1);
    curl_multi_add_handle($mh, $ch2);

    $active = 0;
    do {
        curl_multi_exec($mh, $active);
    } while ($active > 0);

    curl_multi_remove_handle($mh, $ch1);
    curl_multi_remove_handle($mh, $ch2);
    curl_multi_close($mh);

    // Force curl to output results
    fflush(STDERR);
    fflush(STDOUT);

    echo "Ok for $description\n";
}

$options_to_check = array(
    "CURLOPT_STDERR", "CURLOPT_WRITEHEADER", "CURLOPT_FILE", "CURLOPT_INFILE"
);

$target_url = curl_cli_server_start();
foreach($options_to_check as $option) {
    checkForClosedFilePointer($target_url, constant($option), $option);
}

?>
--CLEAN--
<?php @unlink(__DIR__ . '/bug48203.tmp'); ?>
--EXPECTF--
Warning: curl_multi_add_handle(): CURLOPT_STDERR resource has gone away, resetting to stderr in %s on line %d
%A
Warning: curl_multi_add_handle(): CURLOPT_STDERR resource has gone away, resetting to stderr in %s on line %d
%A
Ok for CURLOPT_STDERR

Warning: curl_multi_add_handle(): CURLOPT_WRITEHEADER resource has gone away, resetting to default in %s on line %d

Warning: curl_multi_add_handle(): CURLOPT_WRITEHEADER resource has gone away, resetting to default in %s on line %d
Ok for CURLOPT_WRITEHEADER

Warning: curl_multi_add_handle(): CURLOPT_FILE resource has gone away, resetting to default in %s on line %d

Warning: curl_multi_add_handle(): CURLOPT_FILE resource has gone away, resetting to default in %s on line %d
%AOk for CURLOPT_FILE

Warning: curl_multi_add_handle(): CURLOPT_INFILE resource has gone away, resetting to default in %s on line %d

Warning: curl_multi_add_handle(): CURLOPT_INFILE resource has gone away, resetting to default in %s on line %d
Ok for CURLOPT_INFILE
