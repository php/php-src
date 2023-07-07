<?php

$ch = curl_init("https://localhost");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

$body = curl_exec($ch);

curl_close($ch);

if ($body !== "Caddy is up and running") {
    die("skip test needs Caddy");
}
