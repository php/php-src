#!/bin/bash
if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
  createuser -s postgres
fi
echo '                         
<?php $conn_str .= " user=postgres"; ?>' >> "./ext/pgsql/tests/config.inc"
psql -c 'create database test;' -U postgres