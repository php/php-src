#!/bin/sh
set -ev

mysql -vvv -e "CREATE DATABASE IF NOT EXISTS test"
