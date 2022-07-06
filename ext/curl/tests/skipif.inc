<?php
    if (!extension_loaded("curl")) exit("skip curl extension not loaded");
    if(false === getenv('PHP_CURL_HTTP_REMOTE_SERVER')) {
        if (php_sapi_name() != "cli") {
                die("skip PHP_CURL_HTTP_REMOTE_SERVER env variable is not defined");
        }
    }
