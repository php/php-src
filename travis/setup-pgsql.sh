#!/bin/bash
echo '
<?php $conn_str .= " user=postgres"; ?>' >> "./ext/pgsql/tests/config.inc"
if [ -z "$ARM64" -o -z "$S390X"]; then
  psql -c 'create database test;' -U postgres
fi
