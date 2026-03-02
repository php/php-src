# The ldap extension tests

To ease testing LDAP-Setups we've created a vagrant-setup.

## Prerequisites

You will need [vagrant](https://www.vagrantup.com) on your box.

## Usage

To use it follow these steps:

* Create a `Vagrantfile` with the following content.
* Go to that directory and run "vagrant up"

```Vagrantfile
$setup = <<<SETUP
apt-get update

DEBIAN_FRONTEND=noninteractive aptitude install -q -y slapd ldap-utils

export SLAPPASS=`slappasswd -s password`

echo "dn: olcDatabase={1}hdb,cn=config
changetype: modify
replace: olcSuffix
olcSuffix: dc=nodomain
-
replace: olcRootDN
olcRootDN: dc=admin,dc=nodomain
-
replace: olcRootPW
olcRootPW: ${SLAPPASS}" | ldapmodify -Y EXTERNAL -H ldapi:///

echo "dn: dc=nodomain
objectClass: dcObject
objectClass: organization
o: Example
dc: example

dn: ou=extldap,dc=nodomain
objectClass: organizationalUnit
ou: extldap" | ldapadd -c -x -H ldap://localhost:389 -D "dc=admin,dc=nodomain" -w password
SETUP

Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/trusty64"
  config.vm.network "private_network", ip: "192.168.33.10"
  config.vm.provision "shell", inline: $setup
end
```

Now you will have a virtual machine up and running on IP-Address `192.168.10.33`
listening on port 369 for incoming LDAP-connections. The machine is already
configured to execute the LDAP-Tests.

The next step is to go into the PHP-Source-directory. Configure and make the
source as appropriate.

Before running the LDAP-Tests you need to set some environment-variables:

```bash
export LDAP_TEST_PASSWD="password"
export LDAP_TEST_BASE="ou=extldap,dc=nodomain"
export LDAP_TEST_USER="dc=admin,dc=nodomain"
export LDAP_TEST_HOST=192.168.33.10
```

Now you can run the test-suite by calling `make test`. To test only the
LDAP-Tests, run `make test TESTS=ext/ldap`.

CAVEAT: The current setup does not (yet) test secure connections.

## Old README

Most tests here rely on the availability of an LDAP server configured with TLS.

### Client/Server configuration

OpenLDAP 2.4.31 has been used with the configuration below.

Notes:

1. A self signed certificate can be generated using:

    ```bash
    openssl req -newkey rsa:1024 -x509 -nodes -out server.pem -keyout server.pem -days 3650
    ```

    It is used for testing ldap_start_tls(), which also requires
    `TLS_REQCERT never` in client configuration.

2. An empty LDAP structure is required for the tests to be PASSed (except for
    base and admin)

If you use a debian based distribution, prefer the use of `dpkg-reconfigure`.
Otherwise you may alter these configuration files:

#### (/etc/openldap/)slapd.conf

```txt
TLSCACertificateFile /etc/openldap/ssl/server.pem
TLSCertificateFile /etc/openldap/ssl/server.pem
TLSCertificateKeyFile /etc/openldap/ssl/server.pem
TLSVerifyClient never

# hdb is used instead of bdb as it enables the usage of referrals & aliases
database        hdb
suffix          "dc=my-domain,dc=com"
checkpoint      32      30
rootdn          "cn=Manager,dc=my-domain,dc=com"
rootpw          secret
directory       /var/lib/openldap-data
index   objectClass     eq

authz-regexp
    uid=Manager,cn=digest-md5,cn=auth
    cn=Manager,dc=my-domain,dc=com
```

#### (/etc/openldap/)ldap.conf

```txt
TLS_REQCERT never
```

#### Tests configuration

The following environment variables may be defined:

```txt
LDAP_TEST_HOST (default: localhost)                        Host to connect to
LDAP_TEST_PORT (default: 389)                              Port to connect to
LDAP_TEST_BASE (default: dc=my-domain,dc=com)              Base to use. May be the ldap root or a subtree. (ldap_search_variation6 will fail if a subtree is used)
LDAP_TEST_USER (default: cn=Manager,dc=my-domain,dc=com)   DN used for binding
LDAP_TEST_SASL_USER (default: Manager)                     SASL user used for SASL binding
LDAP_TEST_PASSWD (default: secret)                         Password used for plain and SASL binding
LDAP_TEST_OPT_PROTOCOL_VERSION (default: 3)                Version of LDAP protocol to use
LDAP_TEST_SKIP_BIND_FAILURE (default: true)                Whether to fail the test or not in case binding fails
```

## Credits

* Davide Mendolia (idaf1er@gmail.com)
* Patrick Allaert (patrick.allaert@gmail.com)
* Côme Bernigaud (mcmic@php.net)
