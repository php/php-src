<?php

$ch = curl_init("https://localhost");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);

$body = curl_exec($ch);

if ($body !== "Caddy is up and running") {
    die("skip test needs Caddy");
}
