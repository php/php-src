<?php
if(substr(PHP_OS, 0, 3) !== 'WIN') {
   die('skip windows only test');
}

$connection = @fsockopen('localhost', 1025);
if ($connection === false) {
    die('skip unable to open mailhog send port');
}

$c = curl_init();
curl_setopt($c, CURLOPT_URL, 'http://localhost:8025/api/v2/messages');
curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
$res = curl_exec($c);
curl_close($c);

if (!$res) {
    die("skip could not connect to mailhog api");
}
?>
