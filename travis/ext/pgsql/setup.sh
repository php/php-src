#!/bin/bash
echo '
<?php $conn_str .= " user=postgres"; ?>' >> "./ext/pgsql/tests/config.inc"
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then createuser -s postgres; fi
psql -c 'create database test;' -U postgres
