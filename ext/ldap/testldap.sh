#!/bin/bash
vagrant up
export LDAP_TEST_PASSWD="password"
export LDAP_TEST_BASE="ou=extldap,dc=nodomain"
export LDAP_TEST_USER="dc=admin,dc=nodomain"
export LDAP_TEST_HOST=192.168.33.10

cd ../..
make test TESTS=ext/ldap
cd ext/ldap
