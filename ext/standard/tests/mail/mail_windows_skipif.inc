<?php
if(substr(PHP_OS, 0, 3) !== 'WIN') {
   die('skip windows only test');
}

$connection = @fsockopen('localhost', 1025);
if ($connection === false) {
    die('skip unable to open mailpit send port');
}

if (!file_get_contents("http://localhost:8025/api/v1/info")) {
    die("skip could not get response from mailpit api");
}
?>
