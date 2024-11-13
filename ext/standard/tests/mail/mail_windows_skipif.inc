<?php
if(substr(PHP_OS, 0, 3) !== 'WIN') {
   die('skip windows only test');
}

$connection = @fsockopen('localhost', 25);
if ($connection === false) {
    die('skip unable to open smtp socket');
}

$connection = @fsockopen('localhost', 143);
if ($connection === false) {
    die('skip unable to open imap socket');
}
?>
